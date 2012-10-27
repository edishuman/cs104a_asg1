%{
#include <stdlib.h>
#include <stdbool.h>

#include "auxlib.h"
#include "lyutils.h"

#define YY_USER_ACTION  { scanner_useraction (); }
#define IGNORE(THING)   { }

%}

%option 8bit
%option backup
%option debug
%option ecs
%option nodefault
%option nounput
%option perf-report
%option verbose

LETTER          [A-Za-z_]
DIGIT           [0-9]
NUMBER          {DIGIT}+
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT	    ([Ee][+-]?{DIGIT}+)
TOK_INTCON      ({MANTISSA}{EXPONENT}?)
TOK_NOTINTCON   ({MANTISSA}[Ee][+-]?)
TOK_IDENT       ({LETTER}({LETTER}|{DIGIT})*)
TOK_CHARCON		('([^\\'\n]|\\[\\'"0nt])')
TOK_STRINGCON 	(\"([^\\"\n]|\\[\\'"0nt])*\")
TOK_NOTIDENT	{NUMBER}{LETTER}({NUMBER}|{LETTER})*

%%

"#".*           { scanner_include(); }
[ \t]+          { IGNORE (white space) }
\n              { scanner_newline(); }

void 			{ return yylval_token (TOK_VOID); }
bool			{ return yylval_token (TOK_BOOL); }
char			{ return yylval_token (TOK_CHAR); }
int				{ return yylval_token (TOK_INT); }
string			{ return yylval_token (TOK_STRING); }
struct			{ return yylval_token (TOK_STRUCT); }
if				{ return yylval_token (TOK_IF); }
else			{ return yylval_token (TOK_ELSE); }
while			{ return yylval_token (TOK_WHILE); }
return			{ return yylval_token (TOK_RETURN); }
new				{ return yylval_token (TOK_NEW); }
false			{ return yylval_token (TOK_FALSE); }
true			{ return yylval_token (TOK_TRUE); }
null			{ return yylval_token (TOK_NULL); }
ord				{ return yylval_token (TOK_ORD); }
chr				{ return yylval_token (TOK_CHR); }

{TOK_NOTIDENT}	{ return yylval_token (TOK_NOTIDENT); }
{TOK_IDENT}     { return yylval_token (TOK_IDENT); }
{TOK_INTCON}    { return yylval_token (TOK_INTCON); }
{TOK_CHARCON}	{ return yylval_token (TOK_CHARCON); }
{TOK_STRINGCON}	{ return yylval_token (TOK_STRINGCON); }

"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
"["				{ return yylval_token ('['); }
"]"				{ return yylval_token (']'); }
"{"				{ return yylval_token ('{'); }
"}"				{ return yylval_token ('}'); }
";"             { return yylval_token (';'); }
","				{ return yylval_token (','); }
"."				{ return yylval_token ('.'); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"%"				{ return yylval_token ('%'); }
"!"				{ return yylval_token ('!'); }

"[]"			{ return yylval_token (TOK_ARRAY); }
"=="			{ return yylval_token (TOK_EQ); }
"!="			{ return yylval_token (TOK_NE); }
"<"				{ return yylval_token (TOK_LT); }
"<="			{ return yylval_token (TOK_LE); }
">"				{ return yylval_token (TOK_GT); }
">="			{ return yylval_token (TOK_GE); }

{TOK_NOTINTCON} { scanner_badtoken (yytext);
                  return yylval_token (TOK_INTCON); }

.               { scanner_badchar (*yytext); }

%%

// unset: %option noyywrap
int yywrap (void){
   yy_delete_buffer (YY_CURRENT_BUFFER);
   if (yy_flex_debug) eprintf ("yywrap() return %d\n", true);
   return true;
}































