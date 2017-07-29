/*
 * Lexical Scanner
 *
 * Copyright (c) 2016-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "lexer.h"

void lexer_init (struct lexer *o, const char *buf)
{
	o->start = o->stop = buf;
}

#define HEAD		(*o->stop)
#define GOT(token)	do { return LEXER_ ## token;	} while (0)
#define IGNORE		do { goto start;		} while (0)
#define NEXT		do { ++o->stop;			} while (0)
#define GOTO(label)	do { NEXT; goto x_ ## label;	} while (0)

int lexer_process (struct lexer *o)
{
	o->start = o->stop;
start:
	if (HEAD == '\0')
		GOT (EOI);

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

	GOT (ERROR);
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

	GOT (ID);
x_is_1:
	GOT (IS);
x_or_1:
	GOT (OR);
x_term_1:
	GOT (TERM);
}

#include <stdlib.h>
#include <string.h>

static const size_t size = BUFSIZ;

int lexer_file_init (struct lexer_file *o)
{
	if ((o->buf = malloc (size)) == NULL)
		return 0;

	o->len = 0;
	o->buf[o->len] = '\0';
	lexer_init (&o->lexer, o->buf);

	return 1;
}

void lexer_file_fini (struct lexer_file *o)
{
	free (o->buf);
}

int lexer_file_process (struct lexer_file *o, FILE *f)
{
	int token;
	size_t len;

	token = lexer_process (&o->lexer);

	if (token > LEXER_EOI ||
	    (token == LEXER_EOI && feof (f)))
		return token;

	/* drop processed data */
	o->len = (o->buf + o->len) - o->lexer.start;
	memmove (o->buf, o->lexer.start, o->len);

	len = fread (o->buf + o->len, 1, size - 1 - o->len, f);
	if (ferror (f))
		return LEXER_ERROR;

	o->len += len;
	o->buf[o->len] = '\0';
	lexer_init (&o->lexer, o->buf);

	return lexer_process (&o->lexer);
}
