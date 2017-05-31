CFLAGS = -Wall -pedantic -std=c11 
CC = gcc
MAIN = statyczne dynamiczne dzielone

all: $(MAIN)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

statyczne: main.c libksiazka.a
	$(CC) -static $(CFLAGS) main.c -L. -lksiazka  -o statyczne
	rm -f results.txt
	./statyczne >> results.txt
	
dzielone: main.c libksiazka.so
	$(CC) $(CFLAGS) main.c -L. -lksiazka -o dzielone
	./dzielone >> results.txt

dynamiczne: main.c libksiazka.so
	$(CC) $(CFLAGS) mainDyn.c -L. -lksiazka -o dynamiczne
	./dynamiczne >> results.txt

libksiazka.a: ksiazka.c ksiazka.h
	$(CC) -c -o ksiazka.o ksiazka.c $(CFLAGS)
	ar crs libksiazka.a ksiazka.o

libksiazka.so: ksiazka.c ksiazka.h
	$(CC) -o ksiazka.o -c ksiazka.c $(CFLAGS) -fpic
	$(CC) -shared -o libksiazka.so ksiazka.o $(CFLAGS)
	
.PHONY: clean
clean:
	rm -f *.o $(MAIN) *.gch *.so *.a *.txt
	