#ifndef ZAD1
#define ZAD1

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define comma ,
#define IS_ERROR_NO_PRINT(condition)\
    do{ if(condition){\
        char c[10];\
        fprintf(stderr, "%s\n", strerror_r(errno, &c[0], 10));\
        exit(EXIT_FAILURE);\
    }}while(0)

#ifdef DEBUG
#define IS_ERROR(condition, print)\
    do{ IS_ERROR_NO_PRINT(condition);\
        fprintf(stderr, "TID: %ld ", gettid());\
        fprintf(stderr, print);\
    }while(0)
#else
#define IS_ERROR(condition, print) IS_ERROR_NO_PRINT(condition)
#endif //DEBUG

#define MAX_DIV (10)
#define MAX_NUMBER (1000)

#define num_of_books (10)
int books[num_of_books];
sem_t wait_reader;
sem_t wait_writer;
sem_t change_info;
int current_readers;
int reader_turn;

int writer_is_unlocked;

int print_info;
int num_of_writers;
int times_to_write;
int num_of_readers;
int times_to_read;

void write_books();
void read_books();

long gettid();

void print_info_sem();

#endif //ZAD1
