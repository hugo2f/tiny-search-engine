# Makefile for `crawler`
#
# Hugo, 1/29/2024

# general definitions
C = ../common
L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$C -I$L $(LOGGING)
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
# uncomment to turn on crawler logging
# LOGGING = -DLOG

# program specific
SRCS = crawler.c
OBJS = $(SRCS:.c=.o)
LIBS =
LLIBS = $L/libcs50.a $C/common.a

.PHONY:	all clean test

# default executable to build
all: crawler

# executables
crawler: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# object files also depend on include files
crawler.o: $C/pagedir.h $C/print.h $L/bag.h $L/hashtable.h $L/webpage.h

test: crawler testing.sh
	bash -v ./testing.sh

clean:
	rm -f crawler
	rm -rf ../data ../data_backup
	rm -f *~ *.o
	rm -rf *.dSYM
