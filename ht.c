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

/* Jenkins One-at-a-time hash */
size_t ht_hash (size_t hash, const void *data, size_t size)
{
	const unsigned char *p;
	size_t i;

	for (p = data, i = 0; i < size; ++i) {
		hash += p[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

int ht_init (struct ht *ht, const struct ht_type *type)
{
	ht->type  = type;
	ht->count = 0;
	ht->size  = 4;

	return (ht->table = calloc (ht->size, sizeof (ht->table[0]))) != NULL;
}

void ht_fini (struct ht *ht)
{
	size_t i;

	for (i = 0; i < ht->count; ++i)
		ht->type->free (ht->table[i]);
}

static size_t get_slot (const struct ht_type *type, size_t size,
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

size_t ht_lookup (struct ht *ht, const void *o)
{
	return get_slot (ht->type, ht->size, ht->table, o);
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
	size_t i = ht_lookup (ht, o);

	if (ht->table[i] != NULL) {
		errno = EEXIST;
		return 0;
	}

	if (!resize (ht))
		return 0;

	++ht->count;
	ht->table[ht_lookup (ht, o)] = o;
	return 1;
}
