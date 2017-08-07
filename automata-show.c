/*
 * LR Automata Vizualisation
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <parser/automata-show.h>

static void marker_show (FILE *f)
{
	fprintf (f, " ·");
}

void item_show (const struct item *o, FILE *f)
{
	size_t i;
	const struct rule *r;

	fprintf (f, "  %s →", o->rule->nt->name);

	for (i = 0, r = o->rule; i < o->pos; ++i)
		fprintf (f, " %s", r->prod[i]->name);

	marker_show (f);

	for (; r->prod[i] != NULL; ++i)
		fprintf (f, " %s", r->prod[i]->name);

	fprintf (f, "\n");
}

void item_set_show (const struct ht *o, FILE *f)
{
	size_t i;
	const struct item *item;

	for (i = 0; i < o->size; ++i)
		if ((item = o->table[i]) != NULL)
			item_show (item, f);
}

void arrow_show (const struct arrow *o, FILE *f)
{
	size_t i = ht_index (&o->to->automata->states, o->to);

	fprintf (f, "  %s → %zu\n", o->on->name, i);
}

void arrow_set_show (const struct ht *o, FILE *f)
{
	size_t i;
	const struct arrow *a;

	for (i = 0; i < o->size; ++i)
		if ((a = o->table[i]) != NULL)
			arrow_show (a, f);
}

void state_show (const struct state *o, FILE *f)
{
	size_t i = ht_index (&o->automata->states, o);

	fprintf (f, "state %zu:\n", i);
	item_set_show (&o->items, f);
	fprintf (f, "arrows from %zu:\n", i);
	arrow_set_show (&o->arrows, f);
}

void automata_show (const struct automata *o, FILE *f)
{
	size_t i;
	const struct state *s;

	for (i = 0; i < o->states.size; ++i)
		if ((s = o->states.table[i]) != NULL)
			state_show (s, f);
}
