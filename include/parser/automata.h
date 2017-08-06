/*
 * LR(0) Automata
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PARSER_AUTOMATA_H
#define PARSER_AUTOMATA_H  1

#include <data/ht.h>
#include <parser/grammar.h>

/* item key = (rule, pos) */
struct item {
	const struct rule *rule;  /* owned by grammar */
	size_t pos;
};

/* arrow key = on */
struct arrow {
	const struct symbol *on;  /* owned by grammar  */
	const struct state *to;   /* owned by automata */
};

/* state key = items */
struct state {
	struct ht items;   /* todo: ordered set of items */
	struct ht arrows;  /* unordered set of arrows */
};

struct state *state_alloc (void);
void state_free (void *o);

struct automata;

int state_add_item (struct state *o, struct automata *a,
		    const struct rule *rule, size_t pos);
int state_add_arrow (struct state *o, const struct symbol *on,
		     const struct state *to);

struct automata {
	struct ht items;   /* unordered set of items  */
	struct ht states;  /* unordered set of states */
};

int  automata_init (struct automata *o);
void automata_fini (struct automata *o);

const struct item *automata_add_item (struct automata *o,
				      const struct rule *rule, size_t pos);
const struct state *automata_add_state (struct automata *o, struct state *s);

#endif  /* PARSER_AUTOMATA_H */
