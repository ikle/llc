# Lexical Scanner
#
# Copyright (c) 2016-2017 Alexei A. Smekalkine
#
# SPDX-License-Identifier: BSD-2-Clause

% extra   { #include "rule.h"        }
% type    { struct symbol *symbol;   }
% context { struct grammar *grammar; }

comment*
	: '#' [^\n]*
space*	: [ \t\n]+ ;
id	: [A-Za-z] ('-'? [A-Za-z0-9])* ;
is	: ':' ;
or	: '|' ;
term	: ';' ;
