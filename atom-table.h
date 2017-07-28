/*
 * String Atom Table
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef ATOM_TABLE_H
#define ATOM_TABLE_H  1

struct atom_table *atom_table_alloc (void);
void atom_table_free (struct atom_table *o);

/*
 * The atom_table_lookup function lookups string name in the specified
 * atom table.
 *
 * Returns a pointer to the string in atom table or NULL in case of
 * memory allocation error.
 */
const char *atom_table_lookup (struct atom_table *o, const char *name);

#endif  /* ATOM_TABLE_H */
