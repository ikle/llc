/*
 * LR Automata Building
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <errno.h>
#include <parser/automata.h>
#include <parser/grammar.h>

static
int state_push_item (struct state *o, const struct rule *rule, size_t pos);

static int state_push_symbol (struct state *o, const struct symbol *nt)
{
	size_t i;
	const struct rule *r;

	if (nt == NULL || nt->rules == NULL)
		return 1;

	ht_foreach (i, r, nt->rules)
		if (!state_push_item (o, r, 0) && errno != EEXIST)
			return 0;

	return 1;
}

static
int state_push_item (struct state *o, const struct rule *rule, size_t pos)
{
	return state_add_item (o, rule, pos) &&
	       state_push_symbol (o, rule->prod[pos]);
}

static int state_build (struct state *o)
{
	size_t i;
	const struct item *it;
	const struct symbol *s;
	struct arrow *a;
	const struct state *next;

	ht_foreach (i, it, &o->items)
		if ((s = it->rule->prod[it->pos]) != NULL) {
			if ((a = state_add_arrow (o, s)) == NULL &&
			    errno != EEXIST)
				return 0;

			if (!state_push_item (a->to, it->rule, it->pos + 1) &&
			    errno != EEXIST)
				return 0;
		}

	ht_foreach (i, a, &o->arrows) {
		next = automata_add_state (o->automata, a->to);
		if (next != a->to) {
			state_free (a->to);
			a->to = (void *) next;

			if (next == NULL)
				return 0;
		}
		else
			if (!state_build (a->to))
				return 0;
	}

	return 1;
}

int automata_build (struct automata *a, const struct grammar *g)
{
	struct state *s;

	if ((s = state_alloc (a)) == NULL)
		return 0;

	if (!state_push_symbol (s, g->start) ||
	    automata_add_state (a, s) != s   ||
	    !state_build (s))
		goto error;

	return 1;
error:
	state_free (s);
	return 0;
}
