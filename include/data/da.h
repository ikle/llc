/*
 * Dynamic Array
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DATA_DA_H
#define DATA_DA_H  1

#include <data/type.h>

struct da {
	const struct data_type *type;
	size_t count, size;
	void **table;
};

int  da_init (struct da *da, const struct data_type *type);
void da_fini (struct da *da);

int da_insert (struct da *da, void *o);

#endif  /* DATA_DA_H */
