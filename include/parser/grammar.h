/*
 * Context-Free Grammar
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PARSER_GRAMMAR_H
#define PARSER_GRAMMAR_H  1

#include <data/ht.h>

struct grammar {
	struct ht names;
	struct ht symbols;
	struct symbol *start;
};

/*
 * Grammar Symbol
 *
 * 1. Names are not owned by symbol.
 * 2. Distinct name pointers point to distinct names.
 * 3. Rules are owned by symbol.
 */

struct symbol {
	const char *name;
	struct ht *rules;  /* NULL for terminals */
};

/*
 * Grammar Rule
 *
 * 1. Symbols are not owned by rule.
 * 2. Distinct symbol pointers point to distinct symbols.
 */

struct rule {
	struct symbol *nt;
	const struct symbol *prod[1];  /* NULL-terminated sequence */
};

int  grammar_init (struct grammar *o);
void grammar_fini (struct grammar *o);

const char    *grammar_add_name   (struct grammar *o, const char *name);
struct symbol *grammar_add_symbol (struct grammar *o, const char *name);

int symbol_insert_rule (struct symbol *o, struct rule *rule);

#endif  /* PARSER_GRAMMAR_H */
