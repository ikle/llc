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
#define GOT(token)	do { return LEXER_ ## token;		} while (0)
#define CHECK		do { if (HEAD == '\0') GOT (ERROR);	} while (0)
#define NEXT		do { ++o->stop; CHECK;			} while (0)
#define ACCEPT(token)	do { NEXT; GOT (token);			} while (0)
#define GOTO(label)	do { NEXT; goto label;			} while (0)

int lexer_process (struct lexer *o)
{
	o->start = o->stop;

	CHECK;

	if (HEAD == ' ' || HEAD == '\t' || HEAD == '\n')
		GOTO (SPACE_1);

	if ((HEAD >= 'A' && HEAD <= 'Z') || (HEAD >= 'a' && HEAD <= 'z'))
		GOTO (ID_1);

	if (HEAD ==  ':')
		ACCEPT (IS);

	if (HEAD == '|')
		ACCEPT (OR);

	if (HEAD == ';')
		ACCEPT (TERM);

	GOT (ERROR);
SPACE_1:
	if (HEAD == ' ' || HEAD == '\t' || HEAD == '\n')
		GOTO (SPACE_1);

	GOT (SPACE);
ID_1:
	if (HEAD == '-')
		NEXT;

	if ((HEAD >= 'A' && HEAD <= 'Z') || (HEAD >= 'a' && HEAD <= 'z') ||
	    (HEAD >= '0' && HEAD <= '9'))
		GOTO (ID_1);

	GOT (ID);
}
