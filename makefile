CC=gcc
CFLAGS=-I -g -largtable2
SRC=src/main.c src/bmp.c

shared_secret: $(SRC)
	$(CC) -o shared_secret $(SRC) $(CFLAGS)
