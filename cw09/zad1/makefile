CFLAGS = -Wall -pedantic -std=c99 $(ARGS)
CC = gcc
PROG = zad1
DEP = zad1.h
OBJ = reader.o writer.o zad1.o
LDFLAGS = -lpthread

all: $(PROG)

%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@ 

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


.PHONY: clean 
clean:
	rm -f *.o $(PROG)

