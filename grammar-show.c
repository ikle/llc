/*
 * Context-Free Grammar Visualization
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <cfg/grammar-show.h>

void rule_show (const struct rule *r, FILE *f)
{
	size_t i;

	fprintf (f, "%s →", r->nt->name);

	for (i = 0; r->prod[i] != NULL; ++i)
		fprintf (f, " %s", r->prod[i]->name);

	fprintf (f, "\n");
}

void symbol_show (const struct symbol *s, FILE *f)
{
	size_t i;
	const struct rule *r;

	for (i = 0; i < s->rules->size; ++i)
		if ((r = s->rules->table[i]) != NULL)
			rule_show (r, f);
}

void grammar_show (const struct grammar *o, FILE *f)
{
	size_t i;
	const struct symbol *s;

	printf ("start symbol: %s\n", o->start->name);

	for (i = 0; i < o->symbols.size; ++i)
		if ((s = o->symbols.table[i]) != NULL && s->rules != NULL)
			symbol_show (s, f);
}
