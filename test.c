// $RCSfile: cqsort-string.c,v $$Revision: 1.1 $

//
// Example of using qsort(3c) to sort an array of strings.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printarray (char *label, char **array, size_t nelem) 
{
   for (size_t itor = 0; itor < nelem; ++itor) 
   {
      printf ("%s[%2ld]=\"%s\"\n", label, itor, array[itor]);
   }
}

int strstrcmp (const void *this, const void *that) 
{
   char **thisstr = (char**) this;
   char **thatstr = (char**) that;
   return strcmp (*thisstr, *thatstr);
}

int main (int argc, char **argv) 
{
   printf ("Running %s with %d args\n", *argv, argc);

   char *array[] = 
   {
      "This", "is", "a", "test", "of", "qsort(3)", "with", "strings.",
      "foo", "bar", "baz", "qux",
      "Hello", "World",
   };
   
   const size_t nelem = sizeof array / sizeof *array;

   printarray ("Unsorted", array, nelem);
   qsort (array, nelem, sizeof *array, strstrcmp);
   printarray ("Sorted", array, nelem);

   return EXIT_SUCCESS;
}


/*
//TEST// runprog -x cqsort-string.lis ./cqsort-string
//TEST// mkpspdf cqsort-string.ps cqsort-string.c \
//TEST//     cqsort-string.c.log cqsort-string.lis
*/


