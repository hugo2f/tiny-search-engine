# Makefile for `common`
#
# Hugo, 1/29/2024

# general definitions
L = ../libcs50
LIB = common.a
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
# DEBUG = -ggdb (currently included in CFLAGS)

# program specific
OBJS = pagedir.o print.o index.o word.o
LIBS =
LLIBS = $L/libcs50.a

.PHONY: clean

# main target: build the library
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# object files also depend on include files
pagedir.o: pagedir.h print.h $L/webpage.h
print.o: print.h
index.o: index.h $L/hashtable.h $L/counters.h $L/file.h
word.o: word.h

clean:
	rm -f common.a
	rm -f *~ *.o
	rm -rf *.dSYM