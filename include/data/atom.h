/*
 * Atom Type: Pointer to Unique Object
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DATA_ATOM_H
#define DATA_ATOM_H  1

#include <data/type.h>

int atom_eq (const void *a, const void *b);
size_t atom_hash (const void *o);

#endif  /* DATA_ATOM_H */
