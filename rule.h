/*
 * Language Symbols and Rules
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef RULE_H
#define RULE_H  1

#include <data/rb-tree.h>
#include <data/seq.h>

struct grammar {
	struct rb_tree *set;	/* set of symbols          */
	struct symbol *start;	/* start symbol of grammar */
};

int  grammar_init (struct grammar *o);
void grammar_fini (struct grammar *o);

struct rhs_symbol {
	struct rhs_symbol *next;
	struct symbol *symbol;
};

SEQ_DECLARE (rhs_symbol)

struct rhs {
	struct rhs *next;
	struct rhs_symbol_seq seq;
};

SEQ_DECLARE (rhs)

struct symbol {
	char *name;
	struct rhs_seq seq;
};

static inline int symbol_is_terminal (struct symbol *o)
{
	return rhs_seq_is_empty (&o->seq);
}

/*
 * The grammar_add function lookups symbol name in the specified
 * grammar.
 *
 * Returns a pointer to the symbol in grammar or NULL in case of memory
 * allocation error.
 */
struct symbol *grammar_add (struct grammar *o, const char *name);

/*
 * The rhs_symbol_add function allocates new RHS symbol and insert it
 * to the end of the specified RHS.
 *
 * Returns non-zero on success or zero in case of memory allocation
 * error.
 */
int rhs_symbol_add (struct grammar *g, struct rhs *rhs, const char *name);

/*
 * The rhs_add function allocates new RHS alternative and insert it to
 * the end of the specified LHS.
 *
 * Returns pointer to the new RHS or NULL in case of memory allocation
 * error.
 */
struct rhs *rhs_put (struct symbol *lhs);

#endif  /* RULE_H */
