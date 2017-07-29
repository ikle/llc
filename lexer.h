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

struct lexer_buf {
	const char *start, *stop;  /* matched item */
};

void lexer_buf_init (struct lexer_buf *o, const char *buf);
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

#ifndef NOFILE
#include <stdio.h>

struct lexer_file {
	struct lexer_buf lexer;
	char *buf;
	size_t len;
	void *cookie;
	int (*read) (char *buf, size_t size, void *cookie);
};

int  lexer_file_init (struct lexer_file *o, FILE *f);
void lexer_file_fini (struct lexer_file *o);
int  lexer_file_process (struct lexer_file *o);

#endif  /* NOFILE */

#endif  /* LEXER_H */
