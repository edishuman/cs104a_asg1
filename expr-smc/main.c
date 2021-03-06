
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "astree.h"
#include "emit.h"
#include "lyutils.h"
#include "auxlib.h"

#define CPP "/usr/bin/cpp"

struct options{
   bool dumptree;
   bool echoinput;
};

// Open a pipe from the C preprocessor.
// Exit failure if can't.
// Assignes opened pipe to FILE *yyin.
char *yyin_cpp_command = NULL;
void yyin_cpp_popen (char *filename) {
   yyin_cpp_command = malloc (strlen (CPP) + strlen (filename) + 2);
   assert (yyin_cpp_command != NULL);
   strcpy (yyin_cpp_command, CPP);
   strcat (yyin_cpp_command, " ");
   strcat (yyin_cpp_command, filename);
   yyin = popen (yyin_cpp_command, "r");
   if (yyin == NULL) {
      syserrprintf (yyin_cpp_command);
      exit (get_exitstatus());
   }
}

void yyin_cpp_pclose (void) {
   int pclose_rc = pclose (yyin);
   eprint_status (yyin_cpp_command, pclose_rc);
   if (pclose_rc != 0) set_exitstatus (EXIT_FAILURE);
}


void scan_opts (int argc, char **argv, struct options *options) {
   int option;
   opterr = 0;
   yy_flex_debug = 0;
   yydebug = 0;
   for(;;) {
      option = getopt (argc, argv, "@:ely");
      if (option == EOF) break;
      switch (option) {
         case '@': set_debugflags (optarg);   break;
         case 'e': options->echoinput = true; break;
         case 'l': yy_flex_debug = 1;         break;
         case 'y': yydebug = 1;               break;
         default:  errprintf ("%:bad option (%c)\n", optopt); break;
      }
   }
   if (optind > argc) {
      errprintf ("Usage: %s [-ly] [filename]\n", get_execname());
      exit (get_exitstatus());
   }
   char *filename = optind == argc ? "-" : argv[optind];
   yyin_cpp_popen (filename);
   DEBUGF ('m', "filename = %s, yyin = %p, fileno (yyin) = %d\n",
           filename, yyin, fileno (yyin));
   scanner_newfilename (filename);
}

int main (int argc, char **argv) {
   struct options options = {false, false};
   int parsecode = 0;
   set_execname (argv[0]);
   DEBUGSTMT ('m',
      for (int argi = 0; argi < argc; ++argi) {
         eprintf ("%s%c", argv[argi], argi < argc - 1 ? ' ' : '\n');
      }
   );
   scan_opts (argc, argv, &options);
   scanner_setecho (options.echoinput);
   parsecode = yyparse();
   if (parsecode) {
      errprintf ("%:parse failed (%d)\n", parsecode);
   }else {
      DEBUGSTMT ('a', dump_astree (stderr, yyparse_astree); );
      emit_sm_code (yyparse_astree);
   }
   freeast (yyparse_astree);
   yyin_cpp_pclose();
   return get_exitstatus();
}

// LINTED(static unused)
RCSC(MAIN_C,"$Id: main.c,v 1.16 2012-10-22 14:03:59-07 - - $")


