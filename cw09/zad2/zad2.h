#ifndef ZAD2
#define ZAD2

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
        fprintf(stderr, "%s %s %d\n", strerror_r(errno, &c[0], 10), __func__, __LINE__);\
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

#define IS_ERROR_E(condition, print) IS_ERROR(0 != (errno = condition), print)

#define MAX_DIV (10)
#define MAX_NUMBER (1000)

#define num_of_books (10)
int books[num_of_books];
pthread_mutex_t queue_mutex;
pthread_mutex_t writer_queue_mutex;
pthread_mutex_t reader_queue_mutex;
pthread_cond_t writer_queue_chnage;
pthread_cond_t reader_queue_chnage;

int print_info;
int num_of_writers;
int times_to_write;
int num_of_readers;
int times_to_read;

void write_books();
void read_books();
void print_info_data();
void print_info_queue();
long gettid();

struct queue{
    int* start;
    int current_pos;
    int last_pos;
    int readers[2];
    int current_reader;
}queue;

void queue_init();
int  queue_add_writer();
int  queue_add_reader();
int  queue_get_next_id();
void queue_rem_writer();
void queue_rem_reader();
int queue_is_readers_turn();

#endif //ZAD2
