# Makefile for `crawler`
#
# Hugo, 1/29/2024

# general definitions
L = ../libcs50
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I../common $(LOGGING)
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
# uncomment to turn on crawler logging
# LOGGING = -DLOG

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

$(LLIBS):
	if [[ ! -f $(LLIBS) ]]; then cp "$L/libcs50-given.a" "$(LLIBS)"; fi

test: crawler testing.sh
	bash -v ./testing.sh

clean:
	rm -f crawler
	rm -f *~ *.o
	rm -rf *.dSYM
	rm -rf ../data../data_backup