/*
 * LR Automata to Code Conversion
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <parser/automata-code.h>
#include <parser/automata-show.h>

static size_t state_index (const struct state *s)
{
	return ht_index (&s->automata->states, s);
}

static void arrow_code (const struct arrow *a, FILE *f)
{
	if (a->on->rules != NULL)
		return;

	fprintf (f, "\tcase TOKEN_%s: ret = parse_%zu (c); break;\n",
		 a->on->name, state_index (a->to));
}

static void arrow_set_code (const struct ht *s, FILE *f)
{
	size_t i;
	const struct arrow *a;

	fprintf (f, "\n\tswitch (c->lex (c)) {\n");

	ht_foreach (i, a, s)
		arrow_code (a, f);

	fprintf (f, "\tdefault: return -1;\n");
	fprintf (f, "\t}\n");
}

static void arrow_nt_code (const struct arrow *a, FILE *f)
{
	if (a->on->rules == NULL)
		return;

	fprintf (f, "\tcase NT_%s: return parse_%zu (c);\n",
		 a->on->name, state_index (a->to));
}

static void arrow_set_nt_code (const struct ht *s, FILE *f)
{
	size_t i;
	const struct arrow *a;

	fprintf (f, "\n\tswitch (ret) {\n");

	ht_foreach (i, a, s)
		arrow_nt_code (a, f);

	fprintf (f, "\tdefault: return -1;\n");
	fprintf (f, "\t}\n");
}

static const struct rule *do_reduce (const struct state *s)
{
	const struct rule *rule = NULL;
	int rr = 0, shift = 0;
	size_t i;
	const struct item *it;

	ht_foreach (i, it, &s->items) {
		if (it->rule->prod[it->pos] == NULL) {
			if (rule != NULL)
				rr = 1;

			rule = it->rule;
		}
		else
			shift = 1;
	}

	if (rr)
		fprintf (stderr, "W: reduce/reduce conflict:\n");

	if (shift && rule != NULL)
		fprintf (stderr, "W: shift/reduce conflict:\n");

	if (rr || (shift && rule != NULL))
		item_set_show (&s->items, stderr);

	return shift ? NULL : rule;
}

void state_code (const struct state *s, FILE *f)
{
	const struct rule *rule;

	fprintf (f, "\nstatic int parse_%zu (struct parser *c)\n{\n",
		 state_index (s));

	if ((rule = do_reduce (s)) != NULL)
		fprintf (f, "\treturn NT_%s;\n", rule->nt->name);
	else {
		fprintf (f, "\tint ret;\n");

		arrow_set_code (&s->arrows, f);
		arrow_set_nt_code (&s->arrows, f);

		fprintf (f, "\n\treturn -1;  /* never be here */\n");
	}

	fprintf (f, "}\n");
}

void automata_code (const struct automata *o, FILE *f)
{
	size_t i;
	const struct state *s;

	if (o->start == NULL)
		return;

	ht_foreach (i, s, &o->states)
		fprintf (f, "static int parse_%zu (struct parser *c);\n",
			 state_index (s));

	ht_foreach (i, s, &o->states)
		state_code (s, f);

	fprintf (f, "\nint parse (struct parser *c)\n"
		"{\n"
		"\treturn parse_%zu (c);\n"
		"}\n",
		state_index (o->start));
}
