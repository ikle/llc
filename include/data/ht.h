/*
 * Opening Addressing Hash Table
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DATA_HT_H
#define DATA_HT_H  1

#include <data/type.h>

struct ht {
	const struct data_type *type;
	size_t count, size;
	void **table;
};

int  ht_init (struct ht *ht, const struct data_type *type);
void ht_fini (struct ht *ht);

size_t ht_index (const struct ht *ht, const void *o);
void *ht_lookup (const struct ht *ht, const void *o);
int ht_insert (struct ht *ht, void *o);

#endif  /* DATA_HT_H */
