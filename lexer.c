/*
 * Lexical Scanner
 *
 * Copyright (c) 2016-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <string.h>
#include "lexer.h"

void lexer_buf_init (struct lexer_buf *o, const lexer_input_t *buf)
{
	o->start = o->stop = buf;
}

#define HEAD		(*o->stop)
#define GOT(token)	do { return LEXER_ ## token;	} while (0)
#define IGNORE		do { goto start;		} while (0)
#define NEXT		do { ++o->stop;			} while (0)
#define GOTO(label)	do { NEXT; goto x_ ## label;	} while (0)

#define DEFINE_DATA							\
	const size_t len = o->stop - o->start;				\
	lexer_input_t data[len + 1];					\
									\
	memcpy (data, o->start, len * sizeof (lexer_input_t));		\
	data[len] = 0

int lexer_buf_process (struct lexer_buf *o, union lexer_type *value)
{
	o->start = o->stop;
start:
	if (HEAD == 0)
		return 0;

	if (HEAD == '#')
		GOTO (comment_1);

	if (HEAD == ' ' || HEAD == '\t' || HEAD == '\n')
		GOTO (space_1);

	if ((HEAD >= 'A' && HEAD <= 'Z') || (HEAD >= 'a' && HEAD <= 'z'))
		GOTO (id_1);

	if (HEAD ==  ':')
		GOTO (is_1);

	if (HEAD == '|')
		GOTO (or_1);

	if (HEAD == ';')
		GOTO (term_1);

	return -1;
x_comment_1:
	if (HEAD != 0 && HEAD != '\n')
		GOTO (comment_1);

	IGNORE;
x_space_1:
	if (HEAD == ' ' || HEAD == '\t' || HEAD == '\n')
		GOTO (space_1);

	IGNORE;
x_id_1:
	if (HEAD == '-')
		NEXT;

	if ((HEAD >= 'A' && HEAD <= 'Z') || (HEAD >= 'a' && HEAD <= 'z') ||
	    (HEAD >= '0' && HEAD <= '9'))
		GOTO (id_1);

	{ DEFINE_DATA; value->symbol = grammar_add (o->grammar, data); }

	GOT (ID);
x_is_1:
	GOT (IS);
x_or_1:
	GOT (OR);
x_term_1:
	GOT (TERM);
}

#ifndef NOGENERIC

#include <stdio.h>
#include <stdlib.h>

static const size_t count = BUFSIZ / sizeof (lexer_input_t);

int lexer_init (struct lexer *o, void *cookie, lexer_read_fn *read)
{
	if ((o->buf = malloc (count * sizeof (o->buf[0]))) == NULL)
		return 0;

	o->len = 0;
	o->buf[o->len] = '\0';
	lexer_buf_init (&o->lexer_buf, o->buf);

	o->cookie = cookie;
	o->read = read;

	return 1;
}

void lexer_fini (struct lexer *o)
{
	free (o->buf);
}

int lexer_process (struct lexer *o, union lexer_type *value)
{
	int token;
	size_t len;

	token = lexer_buf_process (&o->lexer_buf, value);

	if (token > 0)
		return token;

	/* drop processed data */
	o->len = (o->buf + o->len) - o->lexer_buf.start;
	memmove (o->buf, o->lexer_buf.start, o->len);

	len = o->read (o->buf + o->len, count - 1 - o->len, o->cookie);
	if (len <= 0)
		return len;

	o->len += len;
	o->buf[o->len] = '\0';
	lexer_buf_init (&o->lexer_buf, o->buf);

	return lexer_buf_process (&o->lexer_buf, value);
}

#ifndef NOFILE

static int file_read (lexer_input_t *buf, size_t count, void *cookie)
{
	size_t len;

	len = fread (buf, sizeof (lexer_input_t), count, cookie);
	if (len == 0)
		return feof (cookie) ? 0 : -1;

	return len;
}

int lexer_file_init (struct lexer *o, FILE *f)
{
	return lexer_init (o, f, file_read);
}

#endif  /* NOFILE */
#endif  /* NOGENERIC */
