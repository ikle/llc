#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include <cfg/grammar.h>
#include <cfg/grammar-show.h>

const char *r0[] = { "S", "E", NULL };
const char *r1[] = { "E", "T", NULL };
const char *r2[] = { "E", "(", "E", ")", NULL };
const char *r3[] = { "T", "n", NULL };
const char *r4[] = { "T", "T", "+", "n", NULL };

const char **G[] = { r0, r1, r2, r3, r4, NULL };

static struct rule *rule_alloc (struct grammar *o, const char *r[])
{
	size_t i, size;
	struct rule *rule;

	if (r[0] == NULL)
		return NULL;

	for (i = 0; r[i] != NULL; ++i) {}

	size = offsetof (struct rule, prod) + sizeof (rule->prod[0]) * i;

	if ((rule = malloc (size)) == NULL)
		goto no_rule;

	if ((rule->nt = grammar_add_symbol (o, r[0])) == NULL)
		goto no_symbol;

	for (i = 0; r[i + 1] != NULL; ++i)
		if ((rule->prod[i] = grammar_add_symbol (o, r[i + 1])) == NULL)
			goto no_symbol;

	rule->prod[i] = NULL;
	return rule;
no_symbol:
	free (rule);
no_rule:
	return NULL;
}

static int load_grammar (struct grammar *o, const char **g[])
{
	struct rule *r;

	for (; *g != NULL; ++g) {
		if ((r = rule_alloc (o, *g)) == NULL)
			goto no_rule;

		if (!symbol_insert_rule (r->nt, r))
			goto no_insert;
	}

	return 1;
no_insert:
	free (r);
no_rule:
	return 0;
}

int main (int argc, char *argv[])
{
	struct grammar g;

	if (!grammar_init (&g))
		err (1, "cannot initialize grammar");

	if (!load_grammar (&g, G))
		err (1, "cannot load grammar");

	grammar_show (&g, stdout);
	grammar_fini (&g);
	return 0;
}