CC=gcc
CFLAGS=-I -g

shared_secret: main.c bmp.c
	$(CC) -o shared_secret main.c bmp.c $(CFLAGS).
