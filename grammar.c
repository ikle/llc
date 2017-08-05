/*
 * Context-Free Grammar
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "grammar.h"

/*
 * Grammar Name
 */

static int name_eq (const void *a, const void *b)
{
	return strcmp (a, b) == 0;
}

static size_t name_hash (const void *o)
{
	return ht_hash (0, o, strlen (o));
}

static const struct data_type name_type = {
	.free	= free,
	.eq	= name_eq,
	.hash	= name_hash,
};

/*
 * Grammar Symbol
 *
 * 1. Names are not owned by symbol.
 * 2. Distinct name pointers point to distinct names.
 * 3. Rules are owned by symbol.
 */

static struct symbol *symbol_alloc (const char *name)
{
	struct symbol *s;

	if ((s = malloc (sizeof (*s))) == NULL)
		return NULL;

	s->name  = name;
	s->rules = NULL;
	return s;
}

static void symbol_free (void *o)
{
	struct symbol *s = o;

	if (s == NULL)
		return;

	if (s->rules != NULL) {
		ht_fini (s->rules);
		free (s->rules);
	}

	free (s);
}

static int symbol_eq (const void *a, const void *b)
{
	const struct symbol *p = a;
	const struct symbol *q = b;

	return p->name == q->name;
}

static size_t symbol_hash (const void *o)
{
	const struct symbol *s = o;

	return ht_hash (0, &s->name, sizeof (s->name));
}

static const struct data_type symbol_type = {
	.free	= symbol_free,
	.eq	= symbol_eq,
	.hash	= symbol_hash,
};

/*
 * Grammar Rule
 *
 * 1. Symbols are not owned by rule.
 * 2. Distinct symbol pointers point to distinct symbols.
 */

static int rule_eq (const void *a, const void *b)
{
	const struct rule *p = a;
	const struct rule *q = b;
	size_t i;

	/* assert (p->nt == q->nt) */

	for (i = 0; p->prod[i] != NULL; ++i)
		if (p->prod[i] != q->prod[i])
			return 0;

	return q->prod[i] == NULL;
}

static size_t rule_hash (const void *o)
{
	const struct rule *r = o;
	size_t hash, i;

	for (hash = 0, i = 0; r->prod[i] != NULL; ++i)
		hash = ht_hash (hash, r->prod[i], sizeof (r->prod[0]));

	return hash;
}

static const struct data_type rule_type = {
	.free	= free,
	.eq	= rule_eq,
	.hash	= rule_hash,
};

/*
 * Grammar Interface
 */

int grammar_init (struct grammar *o)
{
	if (!ht_init (&o->names, &name_type))
		goto no_names;

	if (!ht_init (&o->symbols, &symbol_type))
		goto no_symbols;

	o->start = NULL;
	return 1;
no_symbols:
	ht_fini (&o->names);
no_names:
	return 0;
}

void grammar_fini (struct grammar *o)
{
	ht_fini (&o->symbols);
	ht_fini (&o->names);
}

const char *grammar_add_name (struct grammar *o, const char *name)
{
	const char *n;
	char *copy;

	if ((n = o->names.table[ht_lookup (&o->names, name)]) != NULL)
		return n;

	if ((copy = strdup (name)) == NULL)
		goto no_copy;

	if (!ht_insert (&o->names, copy))
		goto no_insert;

	return copy;
no_insert:
	free (copy);
no_copy:
	return NULL;
}

struct symbol *grammar_add_symbol (struct grammar *o, const char *name)
{
	struct symbol *s, fake = { name };

	if ((s = o->symbols.table[ht_lookup (&o->symbols, &fake)]) != NULL)
		return s;

	if ((name = grammar_add_name (o, name)) == NULL)
		goto no_name;

	if ((s = symbol_alloc (name)) == NULL)
		goto no_symbol;

	if (!ht_insert (&o->symbols, s))
		goto no_insert;

	if (o->start == NULL)
		o->start = s;

	return s;
no_insert:
	symbol_free (s);
no_symbol:
no_name:
	return NULL;
}

static int symbol_init_rules (struct symbol *o)
{
	if (o->rules != NULL)
		return 1;

	if ((o->rules = malloc (sizeof (*o->rules))) == NULL)
		goto no_rules;

	if (!ht_init (o->rules, &rule_type))
		goto no_ht;

	return 1;
no_ht:
	free (o->rules);
	o->rules = NULL;
no_rules:
	return 0;

}

int symbol_insert_rule (struct symbol *o, struct rule *rule)
{
	if (o->name != rule->nt->name) {
		errno = EINVAL;
		return 0;
	}

	if (!symbol_init_rules (o))
		return 0;

	return ht_insert (o->rules, rule);
}
