# Makefile for the "list" program that uses the "list" module.
#
# Derek Marina Avery
#CC=gcc
CFLAGS= -Wall -pedantic -std=c11 -g 
OFILES=queue.o hash.o webpage.o pageio.o indexio.o

all: $(OFILES)
	ar cr ../lib/libutils.a $(OFILES)

%.o: %.c %.h
	gcc $(CFLAGS) -c $<

clean: 
	rm -f pageio.o	webpage.o indexio.o ../lib/libutils.a
