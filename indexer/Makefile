# Makefile for the "list" program that uses the "list" module.
#
# Derek Marina Avery
#CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -I../utils/ -L../lib/ -g
LIBS=-lutils -lcurl
OFILES=queue.o hash.o webpage.o pageio.o

indexer:
	gcc $(CFLAGS) indexer.c $(LIBS) -o $@

clean: 
	rm -f *.o indexer
