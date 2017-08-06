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
 * Items
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
 * Arrow
 */

static int arrow_eq (const void *a, const void *b)
{
	const struct arrow *p = a;
	const struct arrow *q = b;

	return atom_eq (p->on, q->on) && atom_eq (p->to, q->to);
}

static size_t arrow_hash (const void *o)
{
	const struct arrow *p = o;

	return hash (hash (0, &p->on, sizeof (p->on)),
		     &p->to, sizeof (p->to));
}

static const struct data_type arrow_type = {
	.free	= free,
	.eq	= arrow_eq,
	.hash	= arrow_hash,
};

/*
 * State
 */

struct state *state_alloc (void)
{
	struct state *o;

	if ((o = malloc (sizeof (*o))) == NULL)
		goto no_state;

	if (!ht_init (&o->items, &atom_type))
		goto no_items;

	if (!ht_init (&o->arrows, &arrow_type))
		goto no_arrows;

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

int state_add_item (struct state *o, struct automata *a,
		    const struct rule *rule, size_t pos)
{
	const struct item *item;

	if ((item = automata_add_item (a, rule, pos)) == NULL)
		return 0;

	return 0;  /* todo */
}

int state_add_arrow (struct state *o, const struct symbol *on,
		     const struct state *to)
{
	struct arrow *a;

	if ((a = malloc (sizeof (*a))) == NULL)
		goto no_arrow;

	if (!ht_insert (&o->arrows, a))
		goto no_insert;

	return 1;
no_insert:
	free (a);
no_arrow:
	return 0;
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

	return s;
}
