/*
 * Atom Sequence Type
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <data/atom-seq.h>
#include <data/hash.h>

int atom_seq_eq (const void *a, const void *b)
{
	const void **p = (void *) a;
	const void **q = (void *) b;
	size_t i;

	for (i = 0; p[i] != NULL; ++i)
		if (p[i] != q[i])
			return 0;

	return q[i] == NULL;
}

size_t atom_seq_hash (const void *o)
{
	const void **p = (void *) o;
	size_t state, i;

	for (state = 0, i = 0; p[i] != NULL; ++i)
		state = hash (state, p[i], sizeof (p[0]));

	return state;
}
