
// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#define _GNU_SOURCE
#define CPP "/usr/bin/cpp"
#define LINESIZE 1024

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

#include "stringtable.h"

int dump_tree;
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
         case 'l': printf("yy_flex_debug = 1\n");       break;
         case 'y': printf("yydebug = 1\n");	            break;
         default:  errprintf ("%:bad option (%c)\n", optopt); break;
      }
   }
   if (optind > argc) {
      errprintf ("Usage: %s [-ly] [filename]\n", get_execname());
      exit (get_exitstatus());
   }
}

int main (int argc, char **argv) { 
	char *input_file;
	stringtable_ref str_table = new_stringtable();
	
	progname = basename (argv[0]);
	set_execname(argv[0]);
	scan_opts(argc, argv);
	input_file = argv[optind];
	
	if (input_file == NULL) {
	    errprintf ("%:bad input %s\n", input_file);
	    exit(1);
	}
	
    char command[strlen (CPP) + 1 + strlen (input_file) + 1];
    strcpy (command, CPP);
    strcat (command, " ");
    strcat (command, cpp_opt);
    strcat (command, input_file);
    
    FILE *pipe = popen (command, "r");
    if (pipe == NULL) {
       syswarn (command);
    } else {
       cpplines (pipe, input_file, str_table);
       pclose (pipe);
    }
	
	//Determine file name to write output to
	char *p = strrchr(input_file, '.');
	int len = p-input_file;
	char outputfile[len + 5];
	strncpy(outputfile, input_file, len);
	strcat(outputfile, ".str");
	
	//Create file with .str extension and write to it
	FILE *writeto = fopen(outputfile, "w");
	
    debugdump_stringtable(str_table, writeto);
    delete_stringtable(str_table);
    
    fclose(writeto);
    return EXIT_SUCCESS;
}

