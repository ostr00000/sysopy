CFLAGS = -Wall -pedantic -std=c99 -lrt $(ARGS)
CC = gcc
PROG = client server
DEP = const.h


all: $(PROG)

%: %.c $(DEP)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean 
clean:
	rm -f *.o $(PROG)

