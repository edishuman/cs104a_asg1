
MKFILE    = Makefile
DEPSFILE  = ${MKFILE}.deps

#
# Definitions of list of files:
#
HSOURCES  = auxlib.h strhash.h stringtable.h
CSOURCES  = main.c auxlib.c strhash.c stringtable.c
ETCSRC    = README ${MKFILE} ${DEPSFILE}
EXECBIN   = oc
ALLCSRC   = ${CSOURCES}
OBJECTS   = ${ALLCSRC:.c=.o}
ALLSRC    = ${ETCSRC} ${HSOURCES} ${CSOURCES}
TESTINS   = ${wildcard test*.in}
LISTSRC   = ${ALLSRC}

#
# Definitions of the compiler and compilation options:
#
GCC       = gcc -g -O0 -Wall -Wextra -std=gnu99
LINT      = lint -Xa -fd -m -u -x -errchk=%all
MKDEPS    = gcc -MM

#
# The first target is always ``all'', and hence the default,
# and builds the executable images
#
all : ${EXECBIN}

# Build the executable image from the object files.
#
${EXECBIN} : ${OBJECTS}
	${GCC} -o${EXECBIN} ${OBJECTS}

#
# Build an object file form a C source file.
#
%.o : %.c
	${GCC} -c $<

#
# Run lint to check the source files.
#
lint : ${CSOURCES}
	${LINT} ${LINTOPT} ${CSOURCES}

#
# Check sources into an RCS subdirectory.
#
ci : ${ALLSRC} ${TESTINS}
	cid + ${ALLSRC} ${TESTINS}

#
# Clean and spotless remove generated files.
#
clean :
	- rm ${OBJECTS} ${DEPSFILE} *.str oc core

spotless : clean
	- rm ${EXECBIN} List.*.ps List.*.pdf

#
# Build the dependencies file using the C preprocessor
#
deps : ${ALLCSRC}
	@ echo "# ${DEPSFILE} created `date` by ${MAKE}" >${DEPSFILE}
	${MKDEPS} ${ALLCSRC} >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${MAKE} --no-print-directory deps

#
# Everything
#
again :
	gmake --no-print-directory spotless deps ci lint all lis
	


