
MKFILE    = Makefile
DEPSFILE  = ${MKFILE}.deps
NOINCLUDE = ci clean spotless
NEEDINCL  = ${filter ${NOINCLUDE}, ${MAKECMDGOALS}}

#
# Definitions of list of files:
#
HSOURCES  = astree.h astree.rep.h lyutils.h auxlib.h strhash.h stringtable.h
CSOURCES  = astree.c lyutils.c main.c auxlib.c strhash.c stringtable.c
LSOURCES  = scanner.l
YSOURCES  = parser.y
ETCSRC    = README ${MKFILE} ${DEPSFILE}
CLGEN     = yylex.c
HYGEN     = yyparse.h
CYGEN     = yyparse.c
CGENS     = ${CLGEN} ${CYGEN}
ALLGENS   = ${HYGEN} ${CGENS}
EXECBIN   = oc
ALLCSRC   = ${CSOURCES} ${CGENS}
OBJECTS   = ${ALLCSRC:.c=.o}
ALLSRC    = ${ETCSRC} ${YSOURCES} ${LSOURCES} ${HSOURCES} ${CSOURCES}
TESTINS   = ${wildcard test?.in}
LISTSRC   = ${ALLSRC} ${HYGEN}

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

#
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
# Build the scanner.
#
${CLGEN} : ${LSOURCES}
	flex -o${CLGEN} ${LSOURCES}
	- rm lex.backup

#
# Build the parser.
#
${CYGEN} ${HYGEN} : ${YSOURCES}
	bison -dtv -o${CYGEN} ${YSOURCES}

#
# Clean and spotless remove generated files.
#
clean :
	- rm ${OBJECTS} ${ALLGENS} ${DEPSFILE}

spotless : clean
	- rm ${EXECBIN} *.str *.tok

#
# Build the dependencies file using the C preprocessor
#
deps : ${ALLCSRC}
	@ echo "# ${DEPSFILE} created `date` by ${MAKE}" >${DEPSFILE}
	${MKDEPS} ${ALLCSRC} >>${DEPSFILE}

${DEPSFILE} :
	@ touch ${DEPSFILE}
	${MAKE} --no-print-directory deps

ifeq "${NEEDINCL}" ""
include ${DEPSFILE}
endif

