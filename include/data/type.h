/*
 * Data Type
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DATA_TYPE_H
#define DATA_TYPE_H  1

#include <stddef.h>

struct data_type {
	void (*free) (void *o);
	int (*eq) (const void *a, const void *b);
	size_t (*hash) (const void *o);
};

#endif  /* DATA_TYPE_H */
