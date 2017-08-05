/*
 * Opening Addressing Hash Table
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DATA_HT_H
#define DATA_HT_H  1

#include <stddef.h>

size_t ht_hash (size_t hash, const void *data, size_t size);

struct ht_type {
	size_t (*hash) (const void *o);
	int (*eq) (const void *a, const void *b);
	void (*free) (void *o);
};

struct ht {
	const struct ht_type *type;
	size_t count, size;
	void **table;
};

int  ht_init (struct ht *ht, const struct ht_type *type);
void ht_fini (struct ht *ht);

size_t ht_lookup (struct ht *ht, const void *o);
int ht_insert (struct ht *ht, void *o);

#endif  /* DATA_HT_H */
