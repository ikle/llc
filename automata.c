/*
 * LR(0) Automata
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>

#include <data/atom.h>
#include <data/hash.h>
#include <parser/automata.h>

/*
 * Automata Item type
 */

static int item_eq (const void *a, const void *b)
{
	const struct item *p = a;
	const struct item *q = b;

	return atom_eq (p->rule, q->rule) && p->pos == q->pos;
}

static size_t item_hash (const void *o)
{
	const struct item *p = o;

	return hash (hash (0, &p->rule, sizeof (p->rule)),
		     &p->pos, sizeof (p->pos));
}

static const struct data_type item_type = {
	.free	= free,
	.eq	= item_eq,
	.hash	= item_hash,
};

/*
 * State Arrow type
 */

static int arrow_eq (const void *a, const void *b)
{
	const struct arrow *p = a;
	const struct arrow *q = b;

	return atom_eq (p->on, q->on);
}

static size_t arrow_hash (const void *o)
{
	const struct arrow *p = o;

	return atom_hash (p->on);
}

static const struct data_type arrow_type = {
	.free	= free,
	.eq	= arrow_eq,
	.hash	= arrow_hash,
};

/*
 * State
 */

struct state *state_alloc (struct automata *a)
{
	struct state *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		goto no_state;

	if (!ht_init (&o->items, &atom_type))
		goto no_items;

	if (!ht_init (&o->arrows, &arrow_type))
		goto no_arrows;

	o->automata = a;
	return o;
no_arrows:
	ht_fini (&o->items);
no_items:
	free (o);
no_state:
	return NULL;
}

void state_free (void *o)
{
	struct state *s = o;

	if (o == NULL)
		return;

	ht_fini (&s->items);
	ht_fini (&s->arrows);
}

static int state_eq (const void *a, const void *b)
{
	const struct state *p = a;
	const struct state *q = b;

	return ht_eq (&p->items, &q->items);
}

static size_t state_hash (const void *o)
{
	const struct state *p = o;

	return ht_hash (&p->items);
}

static const struct data_type state_type = {
	.free	= state_free,
	.eq	= state_eq,
	.hash	= state_hash,
};

int state_add_item (struct state *o, const struct rule *rule, size_t pos)
{
	const struct item *item;

	if ((item = automata_add_item (o->automata, rule, pos)) == NULL)
		return 0;

	return ht_insert (&o->items, (void *) item);
}

struct arrow *state_add_arrow (struct state *o, const struct symbol *on)
{
	struct arrow fake = { on }, *a;

	if ((a = ht_lookup (&o->arrows, &fake)) != NULL)
		return a;

	if ((a = malloc (sizeof (*a))) == NULL)
		goto no_arrow;

	a->on = on;

	if (!ht_insert (&o->arrows, a))
		goto no_insert;

	if ((a->to = state_alloc (o->automata)) == NULL)
		goto no_state;

	return a;
no_state:
no_insert:
	free (a);
no_arrow:
	return NULL;
}

/*
 * Automata
 */

int automata_init (struct automata *o)
{
	if (!ht_init (&o->items, &item_type))
		goto no_items;

	if (!ht_init (&o->states, &state_type))
		goto no_states;

	o->start = NULL;
	return 1;
no_states:
	ht_fini (&o->items);
no_items:
	return 0;
}

void automata_fini (struct automata *o)
{
	ht_fini (&o->items);
	ht_fini (&o->states);
}

const struct item *automata_add_item (struct automata *o,
				      const struct rule *rule, size_t pos)
{
	struct item *item, fake = {rule, pos};

	if ((item = ht_lookup (&o->items, &fake)) != NULL)
		return item;

	if ((item = malloc (sizeof (*item))) == NULL)
		goto no_item;

	item->rule = rule;
	item->pos  = pos;

	if (!ht_insert (&o->items, item))
		goto no_insert;

	return ht_lookup (&o->items, item);
no_insert:
	free (item);
no_item:
	return NULL;
}

const struct state *automata_add_state (struct automata *o, struct state *s)
{
	const struct state *state;

	if ((state = ht_lookup (&o->states, s)) != NULL)
		return state;

	if (!ht_insert (&o->states, s))
		return NULL;

	if (o->start == NULL)
		o->start = s;

	return s;
}
