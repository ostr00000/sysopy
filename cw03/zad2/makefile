CFLAGS = -Wall -pedantic -std=c99
CC = gcc
PROG = interpreter endelessTime endelessSpace

all: $(PROG)

%: %.c
	$(CC) -o $@ $^ $(CFLAGS) 

.PHONY: clean time
clean:
	rm -f *.o $(PROG)

raport: $(PROG)
	./interpreter rozkazSpace 10 1000
	./interpreter rozkazTime 2 10
