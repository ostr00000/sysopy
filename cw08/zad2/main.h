#ifndef MAIN
#define MAIN

/* requred flag TEST with value SIGUSR1, SIGTERM, SIGKILL, SIGSTOP */
/* requred flag OPERATION with value <- [1,..5] */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/stat.h> 
#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#define IS_ERROR_NO_PRINT(condition)\
    do{ if(condition){\
        char c[10];\
        fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, strerror_r(errno, &c[0], 10));\
        exit(EXIT_FAILURE);\
    }}while(0)

#ifdef DEBUG
#define IS_ERROR(condition, print)\
    do{ IS_ERROR_NO_PRINT(condition);\
        fprintf(stderr, print);\
    }while(0)
#else
#define IS_ERROR(condition, print) IS_ERROR_NO_PRINT(condition)
#endif

#define comma ,
#define STRCAT_THREAD(str,tid) "thread: %ld "str, tid


#define TEXT_SIZE (1024 - sizeof(int))

struct record{
    int id;
    char text[TEXT_SIZE];
};

int file_desc;
int num_of_threads, num_of_records;
char* filename;
char* search_word;
pthread_t* threads;
int finded_flag;

long gettid();

void *search_records(void* arg);

void end();

#if OPERATION == 3 || OPERATION == 5
void catch_signal();
#elif OPERATION == 2 || OPERATION == 4
void mask_signal();
#endif

#endif //MAIN
