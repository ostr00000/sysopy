CFLAGS = -Wall -pedantic -std=c99
CC = gcc
PROG = zad1

all: $(PROG)

%: %.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean 
clean:
	rm -f *.o $(PROG)

