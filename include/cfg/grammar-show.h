/*
 * Context-Free Grammar Visualization
 *
 * Copyright (c) 2010-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef CFG_GRAMMAR_SHOW_H
#define CFG_GRAMMAR_SHOW_H  1

#include <stdio.h>
#include <cfg/grammar.h>

void rule_show    (const struct rule *r,    FILE *f);
void symbol_show  (const struct symbol *s,  FILE *f);
void grammar_show (const struct grammar *o, FILE *f);

#endif  /* CFG_GRAMMAR_SHOW_H */
