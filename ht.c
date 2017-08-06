/*
 * Opening Addressing Hash Table
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <stdlib.h>
#include <data/ht.h>

int ht_init (struct ht *ht, const struct data_type *type)
{
	ht->type  = type;
	ht->count = 0;
	ht->size  = 4;

	return (ht->table = calloc (ht->size, sizeof (ht->table[0]))) != NULL;
}

void ht_fini (struct ht *ht)
{
	size_t i;

	if (ht->type->free == NULL)
		return;

	for (i = 0; i < ht->size; ++i)
		ht->type->free (ht->table[i]);
}

static size_t get_slot (const struct data_type *type, size_t size,
			void **table, const void *o)
{
	const size_t mask = size - 1;  /* size MUST be power of two */
	size_t i;

	for (
		i = type->hash (o) & mask;
		table[i] != NULL && !type->eq (table[i], o);
		i = (i + 1) & mask
	) {}

	return i;
}

size_t ht_index (struct ht *ht, const void *o)
{
	return get_slot (ht->type, ht->size, ht->table, o);
}

void *ht_lookup (struct ht *ht, const void *o)
{
	return ht->table[ht_index (ht, o)];
}

static int resize (struct ht *ht)
{
	if (ht->count <= ht->size / 2)  /* load factor <= 50% */
		return 1;

	const size_t size = ht->size * 2;
	void **table;
	size_t i;
	void *o;

	if ((table = calloc (size, sizeof (table[0]))) == NULL)
		return 0;

	for (i = 0; i < ht->size; ++i)
		if ((o = ht->table[i]) != NULL)
			table[get_slot (ht->type, size, table, o)] = o;

	free (ht->table);

	ht->size  = size;
	ht->table = table;
	return 1;
}

int ht_insert (struct ht *ht, void *o)
{
	size_t i;

	if (!resize (ht))
		return 0;

	i = ht_index (ht, o);

	if (ht->table[i] != NULL) {
		errno = EEXIST;
		return 0;
	}

	++ht->count;
	ht->table[i] = o;
	return 1;
}
