/*
 * String Atom Table
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>

#include "atom-table.h"
#include "rb-tree.h"

struct atom_table *atom_table_alloc (void)
{
	struct rb_tree *o;

	if ((o = rb_alloc ((rb_compare_fn *) strcmp)) == NULL)
		return NULL;

	return (void *) o;
}

void atom_table_free (struct atom_table *at)
{
	struct rb_tree *o = (void *) at;

	rb_free (o, free);
}

const char *atom_table_lookup (struct atom_table *at, const char *name)
{
	struct rb_tree *o = (void *) at;
	struct rb_node *n;
	char *copy;

	if ((n = rb_lookup (o, name)) != NULL)
		return rb_data (n);

	if ((copy = strdup (name)) == NULL)
		goto no_strdup;

	if ((n = rb_insert (o, copy)) == NULL)
		goto no_insert;

	return rb_data (n);
no_insert:
	free (copy);
no_strdup:
	return NULL;
}
