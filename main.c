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
#include "astree.rep.h"
#include "lyutils.h"
#include "mylib.h"
#include "stringtable.h"

#define _GNU_SOURCE
#define CPP "/usr/bin/cpp"
#define LINESIZE 1024
#define YYEOF 0

char *progname;
char cpp_opt[50] = "";
int exit_status = EXIT_SUCCESS;

FILE *writeto_str;

// Print a warning after a failed system call.
void syswarn (char *problem) {
    fflush (NULL);
    fprintf (stderr, "%s: %s: %s\n",
             progname, problem, strerror (errno));
    fflush (NULL);
    exit_status = EXIT_FAILURE;
}

// Chomp the last character from a buffer if it is delim.
void chomp (char *string, char delim) {
    size_t len = strlen (string);
    if (len == 0) return;
    char *nlpos = string + len - 1;
    if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the file.
void cpplines (FILE *pipe, char *input_file, stringtable_ref str_table) {
    int linenr = 1;
    char inputname[LINESIZE];
    strcpy (inputname, input_file);
    
    for (;;) {
       char buffer[LINESIZE];
       char *fgets_rc = fgets (buffer, LINESIZE, pipe);
       if (fgets_rc == NULL) break;
       chomp (buffer, '\n');
       int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                               &linenr, input_file);
       if (sscanf_rc == 2) {
          continue;
       }
       char *savepos = NULL;
       char *bufptr = buffer;
       for (int tokenct = 1;; ++tokenct) {
          char *token = strtok_r (bufptr, " \t\n", &savepos);
          bufptr = NULL;
          if (token == NULL) break;
          intern_stringtable(str_table, token);
       }
       ++linenr;
    }
    
    for (;;) {
		if (yy_flex_debug) {
			fflush (NULL);
			fprintf (stderr, "%s:%d:yylex(): ", input_file, linenr);
		}

		int token = yylex();
		if (token == YYEOF) break;
		if (yy_flex_debug) fflush (NULL);
		
		dump_token();
		/*
		printf("%d%5d.%03d%5d\t%-15s(%s)\n",
		yylval->filenr, yylval->linenr, yylval->offset,
		yylval->symbol, get_yytname(yylval->symbol), yylval->lexinfo);
		*/
		
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
void yyin_cpp_open (char *input_file, stringtable_ref str_table) {
    yyin_cpp_command = malloc(strlen (CPP) + strlen (input_file) + 2);
    strcpy (yyin_cpp_command, CPP);
    strcat (yyin_cpp_command, " ");
    strcat (yyin_cpp_command, cpp_opt);
    strcat (yyin_cpp_command, input_file);
    
    FILE *pipe = popen (yyin_cpp_command, "r");
    yyin = popen (yyin_cpp_command, "r");
    if (pipe == NULL || yyin == NULL) {
       syswarn (yyin_cpp_command);
       pclose(pipe);
    } else {
       cpplines (pipe, input_file, str_table);
       pclose(pipe);
    }
}

void yyin_cpp_pclose (void) {
   int pclose_rc = pclose (yyin);
   eprint_status (yyin_cpp_command, pclose_rc);
}

char *create_file_ext (char *p, char *input_file, char *ext) {
	int len = p-input_file;
	char *outputfile = malloc(len + 5);
	memset(outputfile, 0, len + 5);
	strncpy(outputfile, input_file, len);
	strcat(outputfile, ext);
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
	char *outputfile_str = create_file_ext(p, input_file, ".str");
	char *outputfile_tok = create_file_ext(p, input_file, ".tok");
	
	//Create file with .str/.tok extension and write to it
	writeto_str = fopen(outputfile_str, "w");
	writeto_tok = fopen(outputfile_tok, "w");
	
    yyin_cpp_open(input_file, str_table);

	
	
    debugdump_stringtable(str_table, writeto_str);
    delete_stringtable(str_table);
    
    fclose(writeto_str);
    fclose(writeto_tok);
    yyin_cpp_pclose();
    return EXIT_SUCCESS;
}

