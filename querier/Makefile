# Makefile for `querier`
#
# Hugo, 2/10/2024

# general definitions
C = ../common
L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$C -I$L
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# program specific
OBJS = querier.o
LIBS =
LLIBS = $C/common.a $L/libcs50.a

.PHONY:	all clean test

# default executable to build
all: querier fuzzquery

# executables
querier: querier.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

fuzzquery: fuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# object files also depend on include files
querier.o: $L/hashtable.h $L/counters.h $L/file.h\
$C/pagedir.h $C/print.h $C/index.h $C/word.h $C/pagedir.h

fuzzquery.o: $L/mem.h $L/file.h

test: querier fuzzquery testing.sh
	bash -v ./testing.sh

clean:
	rm -f querier fuzzquery
	rm -f *~ *.o
	rm -rf *.dSYM
