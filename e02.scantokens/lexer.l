%{
#include "tokens.h"

%}

%option 8bit
%option debug
%option ecs
%option nodefault
%option nounput
%option noyywrap
%option perf-report

LETTER          [a-zA-Z_]
DIGIT           [0-9]
IDENT           {LETTER}({LETTER}|{DIGIT})*
NUMBER          {DIGIT}+

%%

{IDENT}         { return IDENT; }
{NUMBER}        { return NUMBER; }
"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
\n              { return '\n'; }
[ \t]+          { /* eat whitespace */ }
"#".*           { /* eat comments */ }
.               { return BAD; }

%%

