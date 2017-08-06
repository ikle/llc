/*
 * Compact Binary Set
 *
 * Copyright (c) 2007-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <data/bitset.h>
#include <data/hash.h>

int bitset_eq (const void *a, const void *b)
{
	const struct bitset *p = a;
	const struct bitset *q = b;

	if (p->count != q->count)
		return 0;

	return memcmp (p->set, q->set, p->count * sizeof (p->set[0])) == 0;
}

size_t bitset_hash (const void *o)
{
	const struct bitset *p = o;

	return hash (0, p->set, p->count * sizeof (p->set[0]));
}

int bitset_is_member (const struct bitset *o, size_t x)
{
	const size_t size = sizeof (o->set[0]) * CHAR_BIT;
	const size_t pos = (x + (CHAR_BIT - 1)) / size;
	const size_t bit = (x + (CHAR_BIT - 1)) % size;

	if (pos >= o->count)
		return 0;

	return (o->set[pos] | (1 << bit)) != 0;
}

static int bitset_prepare (struct bitset *o, size_t count)
{
	uintmax_t *p;

	if (count <= o->count)
		return 1;

	if ((p = realloc (o->set, count)) == NULL)
		return 0;

	memset (p + o->count, 0, (count - o->count) * sizeof (o->set[0]));

	o->count = count;
	o->set = p;
	return 1;
}

static void bitset_shrink (struct bitset *o)
{
	for (; o->count > 0 && o->set[o->count - 1] == 0; --o->count) {}
}

/* o = o U {x} */
int bitset_add (struct bitset *o, size_t x)
{
	const size_t size = sizeof (o->set[0]) * CHAR_BIT;
	const size_t pos = (x + (CHAR_BIT - 1)) / size;
	const size_t bit = (x + (CHAR_BIT - 1)) % size;

	if (!bitset_prepare (o, pos + 1))
		return 0;

	o->set[pos] |= (1 << bit);
	return 1;
}

/* o = o \ {x} */
void bitset_del (struct bitset *o, size_t x)
{
	const size_t size = sizeof (o->set[0]) * CHAR_BIT;
	const size_t pos = (x + (CHAR_BIT - 1)) / size;
	const size_t bit = (x + (CHAR_BIT - 1)) % size;

	if (pos < o->count)
		o->set[pos] &= ~(1 << bit);

	bitset_shrink (o);
}

/* o = o U s */
int bitset_join (struct bitset *o, const struct bitset *s)
{
	size_t i;

	if (o->count < s->count && !bitset_prepare (o, s->count))
			return 0;

	for (i = 0; i < s->count; ++i)
		o->set[i] |= s->set[i];

	return 1;
}

/* o = o \ s */
int bitset_diff (struct bitset *o, const struct bitset *s)
{
	const size_t min = o->count < s->count ? o->count : s->count;
	size_t i;

	for (i = 0; i < min; ++i)
		o->set[i] &= ~s->set[i];

	bitset_shrink (o);
	return 1;
}
