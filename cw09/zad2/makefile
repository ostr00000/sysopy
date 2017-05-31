CFLAGS = -Wall -pedantic -std=c99 -Wextra $(ARGS)
CC = gcc
PROG = zad2
DEP = zad2.h
OBJ = reader.o writer.o zad2.o
LDFLAGS = -lpthread

all: $(PROG)

%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@ 

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


.PHONY: clean 
clean:
	rm -f $(OBJ) $(PROG)

