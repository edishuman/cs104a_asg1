#ifndef __LYUTILS_H__
#define __LYUTILS_H__

// Lex and Yacc interface utility.

#include <stdio.h>

#include "astree.h"
#include "auxlib.h"
#include "mylib.h"

#define YYEOF 0

int yylex (void);
int yyparse (void);
void yyerror (char *message);
const char *get_yytname (int symbol);

char *scanner_filename (int filenr);
void scanner_newfilename (char *filename);
void scanner_badchar (unsigned char bad);
void scanner_badtoken (char *lexeme);
void scanner_newline (void);
void scanner_setecho (bool echoflag);
void scanner_useraction (void);

astree new_parseroot (void);
int yylval_token (int symbol);

void scanner_include (void);

void dump_token (void);

#define YYSTYPE astree
#include "yyparse.h"

// LINTED(static unused)
RCSH(LYUTILS_H,"$Id: lyutils.h,v 1.9 2011-10-06 22:05:30-07 - - $")
#endif

