/*
 * Automata Build Test
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <parser/automata-code.h>
#include <parser/automata-build.h>
#include <parser/automata-show.h>
#include <parser/grammar.h>
#include <parser/grammar-show.h>

#include "grammar-rule.h"

int main (int argc, char *argv[])
{
	struct grammar g;
	struct automata a;

	if (!grammar_init (&g))
		err (1, "cannot initialize grammar");

	if (!load_grammar (&g, G))
		err (1, "cannot load grammar");

	grammar_show (&g, stderr);

	if (!automata_init (&a))
		err (1, "cannot initialize automata");

	if (!automata_build (&a, &g))
		err (1, "cannot build states");

	automata_show (&a, stderr);
	automata_code (&a, stdout);

	grammar_fini (&g);
	automata_fini (&a);
	return 0;
}
