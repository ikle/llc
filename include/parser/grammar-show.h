/*
 * Context-Free Grammar Visualization
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PARSER_GRAMMAR_SHOW_H
#define PARSER_GRAMMAR_SHOW_H  1

#include <stdio.h>
#include <parser/grammar.h>

void rule_show    (const struct rule *r,    FILE *f);
void symbol_show  (const struct symbol *s,  FILE *f);
void grammar_show (const struct grammar *o, FILE *f);

#endif  /* PARSER_GRAMMAR_SHOW_H */
