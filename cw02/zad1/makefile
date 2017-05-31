SHELL := /bin/bash

CFLAGS = -Wall -pedantic -std=c11 
CC = gcc

file = file
file2 = file2
output = wynik.txt

t = ./prog sys generate $(file) $(1) && \
	cp $(file) $(file2) && \
	{ time ./prog sys sort $(file) $(1) 2> /dev/null ; } >> $(output) 2>&1 && echo "sys sort $(1)" >> $(output) && \
	{ time ./prog sys shuffle $(file) $(1) 2> /dev/null ; } >> $(output) 2>&1 && echo "sys shuffle $(1)" >> $(output) && \
	{ time ./prog lib sort $(file2) $(1) 2> /dev/null ; } >> $(output) 2>&1 && echo "lib sort $(1)" >> $(output) && \
	{ time ./prog lib shuffle $(file2) $(1) 2> /dev/null ; } >> $(output) 2>&1 && echo "lib shuffle $(1)" >> $(output) 


all: prog

prog:prog.c
	$(CC) $(CFLAGS) prog.c -o prog
	

.PHONY: clean test
clean:
	rm -f *.o prog $(file) $(file2) $(output)

test:
	rm -f $(output)
	$(call t, 500 4)
	$(call t, 1000 4)
	$(call t, 400 512)
	$(call t, 800 512)
	$(call t, 300 4096)
	$(call t, 600 4096)
	$(call t, 200 8192)
	$(call t, 400 8192)
	rm -f $(file) $(file2)
	


	

