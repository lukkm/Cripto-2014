CC=gcc
CFLAGS=-g -largtable2
SRC=src/main.c src/bmp.c

shared_secret: $(SRC)
	$(CC) $(CFLAGS) -o shared_secret $(SRC) 
