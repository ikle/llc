/*
 * Language Symbols and Rules
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>

#include "rule.h"

static int symbol_compare (const void *a, const void *b)
{
	const struct symbol *p = a, *q = b;

	return strcmp (p->name, q->name);
}

static struct symbol *symbol_alloc (const char *name)
{
	struct symbol *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		goto no_symbol;

	if ((o->name = strdup (name)) == NULL)
		goto no_strdup;

	rhs_seq_init (&o->seq);
	return o;
no_strdup:
	free (o);
no_symbol:
	return NULL;
}

static void rhs_symbol_free (struct rhs_symbol *o)
{
	free (o);
}

static void rhs_free (struct rhs *o)
{
	struct rhs_symbol *p;

	while ((p = rhs_symbol_seq_pop (&o->seq)) != NULL)
		rhs_symbol_free (p);
}

static void symbol_free (void *s)
{
	struct symbol *o = s;
	struct rhs *p;

	if (o == NULL)
		return;

	while ((p = rhs_seq_pop (&o->seq)) != NULL)
		rhs_free (p);

	free (o->name);
	free (o);
}

int grammar_init (struct grammar *o)
{
	if ((o->set = rb_alloc (symbol_compare)) == NULL)
		return 0;

	o->start = NULL;
	return 1;
}

void grammar_fini (struct grammar *o)
{
	rb_free (o->set, symbol_free);
}

/*
 * The grammar_lookup function lookups symbol name in the specified
 * grammar.
 *
 * Returns a pointer to the symbol in grammar or NULL in case of memory
 * allocation error.
 */
struct symbol *grammar_lookup (struct grammar *o, const char *name)
{
	struct rb_node *n;
	struct symbol fake, *s;

	fake.name = (void *) name;

	if ((n = rb_lookup (o->set, &fake)) != NULL)
		return rb_data (n);

	if ((s = symbol_alloc (name)) == NULL)
		goto no_symbol;

	if (o->start == NULL)
		o->start = s;

	if ((n = rb_insert (o->set, s)) == NULL)
		goto no_insert;

	return rb_data (n);
no_insert:
	symbol_free (s);
no_symbol:
	return NULL;
}
