# Makefile for `crawler`
#
# Hugo, 1/29/2024

# general definitions
L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I../common $(DEBUG)
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
DEBUG = -DLOG

# program specific
SRCS = crawler.c ../common/pagedir.c ../common/print.c
OBJS = $(SRCS:.c=.o)
LIBS =
LLIBS = $L/libcs50.a

.PHONY:	all clean test

# default executable to build
all: crawler

# executables
crawler: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# object files also depend on include files
crawler.o: ../common/pagedir.h ../common/print.h $L/bag.h $L/hashtable.h $L/webpage.h
pagedir.o: ../common/pagedir.h ../common/print.h $L/webpage.h
print.o: ../common/print.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: crawler testing.sh
	chmod +x ./testing.sh
	bash -v ./testing.sh

# make valgrind: crawler testing.sh
# 	VALGRIND crawler

clean:
	rm -f crawler
	rm -f *~ *.o
	rm -rf *.dSYM
	rm -rf ../data