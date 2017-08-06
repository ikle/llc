/*
 * Generic Hash
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <data/hash.h>

/* Jenkins One-at-a-time hash */
size_t hash (size_t hash, const void *data, size_t size)
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
