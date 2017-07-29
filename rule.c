/*
 * Language Symbols and Rules
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>

#include "rb-tree.h"
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

	return o;
no_strdup:
	free (o);
no_symbol:
	return NULL;
}

static void symbol_free (void *s)
{
	struct symbol *o = s;

	if (o == NULL)
		return;

	free (o->name);
	free (o);
}

struct rule_table *rule_table_alloc (void)
{
	struct rb_tree *o;

	if ((o = rb_alloc (symbol_compare)) == NULL)
		return NULL;

	return (void *) o;
}

void rule_table_free (struct rule_table *rt)
{
	struct rb_tree *o = (void *) rt;

	rb_free (o, symbol_free);
}

struct symbol *rule_table_lookup (struct rule_table *rt, const char *name)
{
	struct rb_tree *o = (void *) rt;
	struct rb_node *n;
	struct symbol fake, *s;

	fake.name = (void *) name;

	if ((n = rb_lookup (o, &fake)) != NULL)
		return rb_data (n);

	if ((s = symbol_alloc (name)) == NULL)
		goto no_symbol;

	if ((n = rb_insert (o, s)) == NULL)
		goto no_insert;

	return rb_data (n);
no_insert:
	symbol_free (s);
no_symbol:
	return NULL;
}
