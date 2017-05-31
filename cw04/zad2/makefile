CFLAGS = -Wall -pedantic -std=c99 -D debug
CC = gcc
PROG = zad2

all: $(PROG)

%: %.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o $(PROG)

