
CFLAGS = -Wall -pedantic -std=c11 
CC = gcc

all: libksiazka.a libksiazka.so

libksiazka.a: ksiazka.c ksiazka.h
	$(CC) -c ksiazka.c -o ksiazka.o $(CFLAGS)
	ar crs libksiazka.a ksiazka.o

libksiazka.so: ksiazka.c ksiazka.h
	$(CC) -c ksiazka.c -o ksiazka.o $(CFLAGS) -fpic
	$(CC) -shared -o libksiazka.so ksiazka.o $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o *.so *.a