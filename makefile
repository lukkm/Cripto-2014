CC=gcc
CFLAGS=-g -largtable2 -O0
SRC=src/main.c src/bmp.c src/utils.c src/encript.c

shared_secret: $(SRC)
	$(CC) -o shared_secret $(SRC) $(CFLAGS)
