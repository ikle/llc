/*
 * Language Symbols and Rules
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RULE_H
#define RULE_H  1

#include "seq.h"

struct grammar *grammar_alloc (void);
void grammar_free (struct grammar *o);

struct rhs {
	struct rhs *next;
	struct symbol *s;
};

SEQ_DECLARE (rhs)

struct rule {
	struct rule *next;
	struct rhs_seq seq;
};

SEQ_DECLARE (rule)

struct symbol {
	char *name;
	struct rule_seq seq;
};

/*
 * The grammar_lookup function lookups symbol name in the specified
 * grammar.
 *
 * Returns a pointer to the symbol in grammar or NULL in case of memory
 * allocation error.
 */
struct symbol *grammar_lookup (struct grammar *o, const char *name);

#endif  /* RULE_H */
