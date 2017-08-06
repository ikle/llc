/*
 * Atom Type: Pointer to Unique Object
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <data/atom.h>
#include <data/hash.h>

int atom_eq (const void *a, const void *b)
{
	return a == b;
}

size_t atom_hash (const void *o)
{
	return hash (0, &o, sizeof (o));
}
