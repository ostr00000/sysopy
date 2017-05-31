
CFLAGS = -Wall -pedantic -std=c11 
CC = gcc
MAIN = main.exe

DEPS = testTime.h testLista.h testTree.h ksiazka.h dane.h 

OBJ =  testTime.o testLista.o testTree.o ksiazka.o dane.o main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJ)
	$(CC) $(CFLAGS) -o $(MAIN) $^ 

.PHONY: clean
clean:
	rm -f $(OBJ) $(MAIN)