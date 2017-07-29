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

typedef char lexer_input_t;

struct lexer_buf {
	const lexer_input_t *start, *stop;  /* matched item */
};

void lexer_buf_init (struct lexer_buf *o, const lexer_input_t *buf);
int  lexer_buf_process (struct lexer_buf *o);

enum lexer_token {
	LEXER_ERROR = -1,
	LEXER_EOI,		/* end of input */
	LEXER_SPACE,		/* [ \t\n]+ */
	LEXER_ID,		/* [A-Za-z](-?[A-Za-z0-9])* */
	LEXER_IS,		/* : */
	LEXER_OR,		/* | */
	LEXER_TERM,		/* ; */
};

#ifndef NOGENERIC
typedef int lexer_read_fn (lexer_input_t *buf, size_t count, void *cookie);

struct lexer {
	struct lexer_buf lexer;
	lexer_input_t *buf;
	size_t len;
	void *cookie;
	lexer_read_fn *read;
};

int  lexer_init (struct lexer *o, void *cookie, lexer_read_fn *read);
void lexer_fini (struct lexer *o);
int  lexer_process (struct lexer *o);

#ifndef NOFILE
#include <stdio.h>

int lexer_file_init (struct lexer *o, FILE *f);

#endif  /* NOFILE */
#endif  /* NOGENERIC */

#endif  /* LEXER_H */
