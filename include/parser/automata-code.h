/*
 * LR Automata to Code Conversion
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PARSER_AUTOMATA_CODE_H
#define PARSER_AUTOMATA_CODE_H  1

#include <stdio.h>
#include <parser/automata.h>

void state_code    (const struct state *o,    FILE *f);
void automata_code (const struct automata *o, FILE *f);

#endif  /* PARSER_AUTOMATA_CODE_H */
