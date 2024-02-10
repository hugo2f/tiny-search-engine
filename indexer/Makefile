# Makefile for `indexer`
#
# Hugo, 2/6/2024

# general definitions
C = ../common
L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$C -I$L
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

# program specific
OBJS = indexer.o indextest.o
LIBS =
LLIBS = $C/common.a $L/libcs50.a

.PHONY:	all clean test

# default executable to build
all: indexer indextest

# executables
indexer: indexer.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

indextest: indextest.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# object files also depend on include files
indexer.o: $C/index.h $L/hashtable.h $L/set.h $C/pagedir.h $C/print.h
indextest.o: $C/index.h $C/pagedir.h

test: indexer indextest testing.sh
	bash -v ./testing.sh

clean:
	rm -f indexer indextest
	rm -rf ../indexes ../indexes_backup
	rm -f *~ *.o
	rm -rf *.dSYM
