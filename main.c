// $Id: cppstrtok.c,v 1.14 2011-10-03 20:20:08-07 - - $

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

#include "stringtable.h"


int exit_status = EXIT_SUCCESS;
char *progname;

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
void cpplines (FILE *pipe, char *filename, stringtable_ref str_table) {
    int linenr = 1;
    char inputname[LINESIZE];
    strcpy (inputname, filename);
    for (;;) {
       char buffer[LINESIZE];
       char *fgets_rc = fgets (buffer, LINESIZE, pipe);
       if (fgets_rc == NULL) break;
       chomp (buffer, '\n');
       //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
       // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
       int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                               &linenr, filename);
       if (sscanf_rc == 2) {
          //printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, filename);
          continue;
       }
       char *savepos = NULL;
       char *bufptr = buffer;
       for (int tokenct = 1;; ++tokenct) {
          char *token = strtok_r (bufptr, " \t\n", &savepos);
          bufptr = NULL;
          if (token == NULL) break;
          intern_stringtable(str_table, token);
          //printf ("token %d.%d: [%s]\n", linenr, tokenct, token);
       }
       ++linenr;
    }
}

int main (int argc, char **argv) {
	//fileptr determined by user, use it as input for debugdumpstrtable
	//depending on getopt

	stringtable_ref str_table = new_stringtable();
	

    progname = basename (argv[0]);
    for (int argi = 1; argi < argc; ++argi) {
       char *filename = argv[argi];
       char command[strlen (CPP) + 1 + strlen (filename) + 1];
       strcpy (command, CPP);
       strcat (command, " ");
       strcat (command, filename);
       printf ("command=\"%s\"\n", command);
       FILE *pipe = popen (command, "r");
       if (pipe == NULL) {
          syswarn (command);
       }else {
          cpplines (pipe, filename, str_table);
          pclose (pipe);
       }
    }
    
    debugdump_stringtable(str_table, stdout);
    delete_stringtable(str_table);
    
    return EXIT_SUCCESS;
}

