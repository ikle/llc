/*
 * Context-Free Grammar Test
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include <parser/grammar.h>
#include <parser/grammar-show.h>

#include "grammar-rule.h"

int main (int argc, char *argv[])
{
	struct grammar g;

	if (!grammar_init (&g))
		err (1, "cannot initialize grammar");

	if (!load_grammar (&g, G))
		err (1, "cannot load grammar");

	grammar_show (&g, stdout);
	grammar_fini (&g);
	return 0;
}
