// $Id: strerror.c,v 1.6 2011-10-18 19:34:56-07 - - $

//
// NAME
//    strerror - program which wraps the function strerror(3c)
//
// SYNOPSIS
//    strerror errno
//
// OPERANDS
//    Takes one operand which must be a sequence of digits
//    representing a valid errno(3c) code.
//
// DESCRIPTION
//    Prints out the text message associated with a given
//    error code.
//

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *progname = NULL;
void usage_exit (void) {
   fprintf (stderr, "Usage: %s errno\n", progname);
   exit (EXIT_FAILURE);
}

int main (int argc, char **argv) {

   progname = basename (argv[0]);
   if (argc != 2) usage_exit ();

   char *endptr = NULL;
   // LINTED (assignment of 64-bit integer to 32-bit integer)
   int error = strtol (argv[1], &endptr, 10);
   if (argv[1][0] == '\0' || endptr[0] != '\0') usage_exit ();

   printf ("strerror(%d) = \"%s\"\n", error, strerror (error));
   return EXIT_SUCCESS;
}


/*
*****************************************************************

C programs start with #includes to access various header files.

The main function always returns an int and must have a return
statement in it.  `EXIT_SUCCESS' has the value 0 and is returned
when the program thinks it has succeeded at its mission.
`EXIT_FAILURE' has the value 1.  When the program thinks it has
failed, it returns a non-zero exit code.  This value is always
effectively in the range 1..255 inclusive.  If a value outside
the range 0..255 is returned, the value is taken modulo 256.
Note that 256%256==0, -1%256==255, etc.

`argv' is the argument vector, like args in Java.  `argc' is the
dimension of `argv'.  `argv[0]' is the name of the program being
run.  One should use the basename when printing error messages.

`strerror' accepts a system error code and returns a string
containing a text error message.  See:  `man -s 3c strerror'.
If the argument is not a valid error number, it returns the string
"Unknown error" and sets errno to EINVAL.

`strtol' converts a string containing digits into an integer.
See:  `man -s 3c strtol'.

If the argument is not numeric, no message is printed.  `strtol'
returns 0.

*****************************************************************
*/

//TEST// mkpspdf strerror.ps strerror.c*

