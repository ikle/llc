/*
 * Dynamic Array
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>
#include <data/da.h>

int da_init (struct da *da, const struct data_type *type)
{
	da->size = 2;

	if ((da->table = calloc (da->size, sizeof (da->table[0]))) == NULL)
		return 0;

	da->type  = type;
	da->count = 0;
	return 1;
}

void da_fini (struct da *da)
{
	size_t i;

	for (i = 0; i < da->count; ++i)
		da->type->free (da->table[i]);
}

static int resize (struct da *da)
{
	if (da->count < da->size)
		return 1;

	const size_t size = da->size * 2;
	void **table;

	if ((table = realloc (da->table, sizeof (table[0]) * size)) == NULL)
		return 0;

	memset (table + da->size, 0, sizeof (table[0]) * da->size);

	da->size  = size;
	da->table = table;
	return 1;
}

int da_insert (struct da *da, void *o)
{
	if (!resize (da))
		return 0;

	da->table[da->count++] = o;
	return 1;
}
