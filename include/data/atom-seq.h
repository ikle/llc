/*
 * Atom Sequence Type
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef DATA_ATOM_SEQ_H
#define DATA_ATOM_SEQ_H  1

#include <data/atom.h>

/*
 * Atom sequence is NULL-terminated string of atoms
 */
int atom_seq_eq (const void *a, const void *b);
size_t atom_seq_hash (const void *o);

#endif  /* DATA_ATOM_SEQ_H */
