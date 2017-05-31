CFLAGS = -Wall -pedantic -std=c99 -lm
CC = gcc
PROG = master slave

PRINT = printme.pdf printme.ps output.png

all: $(PROG)

%: %.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean 
clean:
	rm -f *.o $(PROG) $(PRINT) data

