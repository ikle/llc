/*
 * Language Symbols and Rules
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RULE_H
#define RULE_H  1

struct rule_table *rule_table_alloc (void);
void rule_table_free (struct rule_table *o);

struct symbol {
	char *name;
};

/*
 * The rule_table_lookup function lookups symbol name in the specified
 * rule table.
 *
 * Returns a pointer to the symbol in rule table or NULL in case of
 * memory allocation error.
 */
struct symbol *rule_table_lookup (struct rule_table *o, const char *name);

#endif  /* RULE_H */
