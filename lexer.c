/*
 * Lexical Scanner
 *
 * Copyright (c) 2016-2017 Alexei A. Smekalkine
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "lexer.h"

int lexer_init (struct lexer *o, char *buf, size_t len)
{
	if (buf[len] != '\0')
		return 0;

	o->start = o->stop = buf;
	return 1;
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
