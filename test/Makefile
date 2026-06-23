# Makefile for the "list" program that uses the "list" module.
#
# Derek Marina Avery
#CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -I../utils/ -L../lib/ -g
LIBS=-lutils -lcurl
OFILES=queue.o hash.o webpage.o pageio.o

test_pageio:
	gcc $(CFLAGS) test_pageio.c $(LIBS) -o $@

test_indexio:
	gcc $(CFLAGS) test_indexio.c $(LIBS) -o $@

clean: 
	rm -f *.o test_pageio test_indexio
