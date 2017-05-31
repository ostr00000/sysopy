CFLAGS = -Wall -Wextra -pedantic -std=c99 $(ARGS)
CC = gcc
PROG = server client
LDFLAGS = -lpthread
DEP = const.h
OBJ = const.o

all: $(PROG)

%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@ 

$(PROG): %: %.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

PHONY: clean 
clean:
	rm -f *.o $(PROG)

