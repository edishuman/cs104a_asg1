// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

#include "astree.h"
#include "emit.h"
#include "lyutils.h"

#include "stringtable.h"

#define _GNU_SOURCE
#define CPP "/usr/bin/cpp"
#define LINESIZE 1024
#define YYEOF 0

char *progname;
char cpp_opt[50] = "";
int exit_status = EXIT_SUCCESS;

// Print a warning after a failed system call.
void syswarn (char *problem) {
    fflush (NULL);
    fprintf (stderr, "%s: %s: %s\n",
             progname, problem, strerror (errno));
    fflush (NULL);
    exit_status = EXIT_FAILURE;
}

// Run cpp against the lines of the file.
void cpplines (char *input_file, stringtable_ref str_table) {
    int linenr = 1;
    char inputname[LINESIZE];
    strcpy (inputname, input_file);
    for (;;) {
       if (yy_flex_debug) {
         fflush (NULL);
         fprintf (stderr, "%s:%d:yylex(): ", input_file, linenr);
      }
      
      int token = yylex();
      if (token == YYEOF) break;
      if (yy_flex_debug) fflush (NULL);
      
      //yyprint(stdout, 
      //printf("token no: %d\t", token);
      //printf("string token: %s\n", get_yytname (token));
      //intern_stringtable(str_table, token); //<-------
      ++linenr;
    }
}

void scan_opts (int argc, char **argv) {
	int option;
	opterr = 0;

	for(;;) {
		option = getopt (argc, argv, "@:D:ly");
		if (option == EOF) break;
		switch (option) {
		 case '@': set_debugflags (optarg); 			break;
		 case 'D': sprintf(cpp_opt, "-D %s ", optarg); 	break;
		 case 'l': yy_flex_debug = 1;         			break;
		 case 'y': yydebug = 1;               			break;
		 default:  errprintf ("%:bad option (%c)\n", optopt); break;
		}
	}
	if (optind > argc) {
		errprintf ("Usage: %s [-ly] [filename]\n", get_execname());
		exit (get_exitstatus());
	}
}

char *yyin_cpp_command = NULL;
void yyin_cpp_popen (char *input_file, stringtable_ref str_table) {
    yyin_cpp_command = malloc(strlen (CPP) + strlen (input_file) + 2);
    strcpy (yyin_cpp_command, CPP);
    strcat (yyin_cpp_command, " ");
    strcat (yyin_cpp_command, cpp_opt);
    strcat (yyin_cpp_command, input_file);
    
    yyin = popen (yyin_cpp_command, "r");
    if (yyin == NULL) {
       syswarn (yyin_cpp_command);
    } else {
       cpplines (input_file, str_table);
    }
}

void yyin_cpp_pclose (void) {
   int pclose_rc = pclose (yyin);
   eprint_status (yyin_cpp_command, pclose_rc);
}

char *create_file_ext (char *p, char *input_file) {
	int len = p-input_file;
	char *outputfile = malloc(len + 5);
	memset(outputfile, 0, len + 5);
	strncpy(outputfile, input_file, len);
	strcat(outputfile, ".str");
	return outputfile;
}

int main (int argc, char **argv) {
	yy_flex_debug = 0;
	progname = basename (argv[0]);
	set_execname(argv[0]);
	scan_opts(argc, argv);
	char *input_file = argv[optind];
	char *p;
	
	if (input_file == NULL ) {
	    errprintf ("Usage: %s [-ly] [-@ flag ...] [-D string] [filename]\n", 
	    			get_execname());
	    exit(1);
	}
	
	//Determine file name to write output to
	p = strrchr(input_file, '.');
	
	if (p == NULL || (strncmp(p, ".oc", 128) != 0)) {
	   errprintf ("%s: file format not recognized.\n", get_execname());
	   exit(1);
	}
	
	stringtable_ref str_table = new_stringtable();
	
	//Auxiliary functions to make code more modular
	char *outputfile = create_file_ext(p, input_file);
    yyin_cpp_popen(input_file, str_table);

	//Create file with .str extension and write to it
	FILE *writeto = fopen(outputfile, "w");
	
    debugdump_stringtable(str_table, writeto);
    delete_stringtable(str_table);
    
    yyin_cpp_pclose();
    return EXIT_SUCCESS;
}

