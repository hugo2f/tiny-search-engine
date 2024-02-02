# Makefile for `common`
#
# Hugo, 1/29/2024

# general definitions
L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
# DEBUG = -ggdb (currently included in CFLAGS)

# program specific
SRCS = pagedir.c print.c
OBJS = $(SRCS:.c=.o)
LIBS =
LLIBS = $L/libcs50.a

.PHONY: clean

# object files also depend on include files
pagedir.o: pagedir.c $L/webpage.h
print.o: print.c

clean:
	rm -f *~ *.o
	rm -rf *.dSYM