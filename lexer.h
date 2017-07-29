/*
 * Lexical Scanner
 *
 * Copyright (c) 2016-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef LEXER_H
#define LEXER_H  1

#include <stddef.h>

struct lexer {
	const char *start, *stop;  /* matched item */
};

void lexer_init (struct lexer *o, const char *buf);
int lexer_process (struct lexer *o);

enum lexer_token {
	LEXER_ERROR = 0,
	LEXER_SPACE,		/* [ \t\n]+ */
	LEXER_ID,		/* [A-Za-z](-?[A-Za-z0-9])* */
	LEXER_IS,		/* : */
	LEXER_OR,		/* | */
	LEXER_TERM,		/* ; */
};

#endif  /* LEXER_H */
