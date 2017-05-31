CFLAGS = -Wall -Wextra -pedantic -std=c99 $(ARGS) 
LDFLAGS = -lpthread -lrt
CC = gcc
PROG =  barber clients
DEP = const.h
OBJ = const.o


all: $(PROG)

%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@ 

$(PROG): %: %.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


.PHONY: clean 
clean:
	rm -f *.o $(PROG) process*.txt

