
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"
#define YYEOF 0

extern FILE* yyin;
extern int yy_flex_debug;
extern char *yytext;
extern int yylex (void);


void scanfile (char *filename) {
   int linenr = 1;
   for (;;) {
      if (yy_flex_debug) {
         fflush (NULL);
         fprintf (stderr, "%s:%d:yylex(): ", filename, linenr);
      }
      int token = yylex();
      if (yy_flex_debug) fflush (NULL);
      switch (token) {
         case YYEOF:
            printf ("END OF FILE\n");
            return;
         case NOTIDENT:
         	printf ("NOTIDENT = \"%s\"\n", yytext);
            break;
         case IDENT:
            printf ("IDENT = \"%s\"\n", yytext);
            break;
         case NUMBER:
            printf ("NUMBER = \"%s\"\n", yytext);
            break;
         case '+':
         case '-':
         case '*':
         case '/':
            printf ("OPERATOR = \"%s\"\n", yytext);
            break;
         case '\n':
            printf ("NEWLINE\n");
            ++linenr;
            break;
         default:
            printf ("BAD TOKEN \"%s\"\n", yytext);
      }
   }
}

int main (int argc, char **argv) {
   char *progname = basename (argv[0]);
   int exit_status = EXIT_SUCCESS;
   
   yy_flex_debug = 0;
   for (;;) {
      int opt = getopt (argc, argv, "l");
      if (opt == EOF) break;
      switch (opt) {
         case 'l': yy_flex_debug = 1;
      }
   }

   if (optind >= argc) {
      scanfile ("-");
   }else {
      for (int argi = optind; argi < argc; ++argi) {
         char *filename = argv[argi];
         yyin = fopen (filename, "r");
         if (yyin == NULL) {
            fflush (NULL);
            fprintf (stderr, "%s: %s: %s\n",
                     progname, filename, strerror (errno));
            fflush (NULL);
            exit_status = EXIT_FAILURE;
         }else {
            scanfile (filename);
            fclose (yyin);
         }
      }
   }

   return exit_status;
}

