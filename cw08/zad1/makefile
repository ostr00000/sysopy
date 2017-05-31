CFLAGS = -Wall -pedantic -std=c99 $(ARGS)
CC = gcc
PROG = asyn_cancel syn_cancel detach
ADD = filegen
LDFLAGS = -lpthread 
DEP = main.h
OBJ = main.o

all: $(PROG) $(ADD)

%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@ 

$(PROG): %: %.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%: %.c $(DEP)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean 
clean:
	rm -f *.o $(PROG) $(ADD)

