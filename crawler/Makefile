# Makefile for the "list" program that uses the "list" module.
#
# Derek Marina Avery
#CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -I../utils/ -L../lib/ -g
LIBS=-lutils -lcurl
OFILES=queue.o hash.o webpage.o pageio.o

crawler:
	gcc $(CFLAGS) crawler.c $(LIBS) -o $@

#%.o: %.c %.h
#	gcc $(CFLAGS) -c $<

clean: 
	rm -f *.o crawler
