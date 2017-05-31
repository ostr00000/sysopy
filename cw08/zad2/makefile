# required flag OPERATION, TEST
OPERATION = -D OPERATION=6 # 1, 2, 3, 4, 5, 6
TEST = -D TEST=SIGUSR1 # SIGUSR1, SIGKILL, SIGTERM, SIGSTOP

CFLAGS = -Wall -pedantic -std=c99 $(ARGS) -D DEBUG
CC = gcc
PROG = asyn_cancel syn_cancel detach
ADD = filegen
LDFLAGS = -lpthread 
DEP = main.h
OBJ = main.o

all: $(PROG) $(ADD)

%.o: %.c $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@ $(OPERATION) $(TEST)

$(PROG): %: %.o $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%: %.c $(DEP)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean 
clean:
	rm -f *.o $(PROG) $(ADD)

