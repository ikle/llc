/*
 * LR Automata Building
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PARSER_AUTOMATA_BUILD_H
#define PARSER_AUTOMATA_BUILD_H  1

#include <parser/automata.h>

int automata_build (struct automata *a, const struct grammar *g);

#endif  /* PARSER_AUTOMATA_BUILD_H */
