/*
 * LR Automata Vizualisation
 *
 * Copyright (c) 2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef PARSER_AUTOMATA_SHOW_H
#define PARSER_AUTOMATA_SHOW_H  1

#include <stdio.h>
#include <parser/automata.h>

void item_show     (const struct item *o, FILE *f); 
void item_set_show (const struct ht *o,   FILE *f);

void arrow_show     (const struct arrow *o, FILE *f);
void arrow_set_show (const struct ht *o,    FILE *f);

void state_show    (const struct state *o,    FILE *f);
void automata_show (const struct automata *o, FILE *f);

#endif  /* PARSER_AUTOMATA_SHOW_H */
