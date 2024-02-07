# Makefile for `indexer`
#
# Hugo, 2/6/2024

# general definitions
C = ../common
L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I../common $(LOGGING)
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
# uncomment to turn on indexer logging
# LOGGING = -DLOG

# program specific
OBJS = indexer.o indextest.o
LIBS =
LLIBS = $L/libcs50.a $C/common.a

.PHONY:	all clean test

# default executable to build
all: indexer indextest

# executables
indexer: indexer.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

indextest: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# object files also depend on include files TODO
indexer.o: $L/hashtable.h $L/set.h $C/pagedir.h $C/print.h $C/index.h

test: indextest testing.sh
	bash -v ./testing.sh

clean:
	rm -f indexer indextest
	rm -f *~ *.o
	rm -rf *.dSYM
