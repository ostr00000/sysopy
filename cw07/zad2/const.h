#ifndef CONST
#define CONST

/*
DEBUG == 1 - print step messages
DEBUG == 2 - print                 shared memory and semaphores
DEBUG == 3 - print step messages + shared memory and semaphores
OUTPUT - create file and write stderr => file
SLEEP=VAL- now haircut take VAL sec
*/

#define _XOPEN_SOURCE 700
#include <sys/types.h>  //key_t
#include <semaphore.h>  //sem_open
#include <fcntl.h>  //O_CREAT
#include <sys/mman.h>   //mmap
#include <string.h> //strerror
#include <errno.h>  //errno
#include <stdio.h>  //printf
#include <stdlib.h> //strtol
#include <time.h>   //clock_gettime
#include <signal.h> //signal
#include <unistd.h> //getpid
#include <sys/wait.h>   //wait

#ifndef MAX_TIME_SLEEP 
#define MAX_TIME_SLEEP 5 /* when client see full queue they go to sleep */
#endif

#define BARBER_SEM_KEY ("/barber_sem")
#define BARBER_SHM_KEY ("/barber_shm")

/*
IS_ERROR3(left, right, str);
    IS_ERROR(left, str);
    IS_ERROR3_VAL(left, right, str, val);
        IS_ERROR_VAL(left, str, val);
*/


#ifdef DEBUG
#define IS_ERROR3(left, right, str_suc)\
    do{ if(left == right){\
        fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, strerror(errno));\
        exit(EXIT_FAILURE);\
    }else{\
        fprintf(stderr, "%s", str_suc);\
    }}while(0)

#define IS_ERROR3_VAL(left, right, str_suc, val)\
    do{ IS_ERROR3(left, right, str_suc);\
        fprintf(stderr, "%d\n", val);\
    }while(0)

#define AT_EXIT(fun)\
    do{ if(0 != atexit(fun)){\
        fprintf(stderr, "%s:%d: cannot set atexit function\n", __func__, __LINE__);\
        (fun)();\
        exit(EXIT_FAILURE);\
    }else{\
        fprintf(stderr, "function atexit set correctly\n");\
    }}while(0)

#else
#define IS_ERROR3(left, right, str_suc)\
    do{ if(left == right){\
        fprintf(stderr, "%s\n", strerror(errno));\
        exit(EXIT_FAILURE);\
    }}while(0)

#define IS_ERROR3_VAL(left, right, str_suc, val) IS_ERROR3(left, right, str_suc)

#define AT_EXIT(fun)\
    do{ if(0 != atexit(fun)){\
        fprintf(stderr, "cannot set atexit function\n");\
        (fun)();\
        exit(EXIT_FAILURE);\
    }}while(0)

#endif //DEBUG

#define IS_ERROR(fun, str_suc) IS_ERROR3(-1, fun, str_suc)

#define IS_ERROR_VAL(fun, str_suc, val) IS_ERROR3_VAL(-1, fun, str_suc, val)

#define SEM_NUM (6)
extern char* suffix[SEM_NUM];

enum semaphore{BARBER_NOT_SLEEP, CLIENT_READY, PAY_BARBER, 
               NEVER_SURRENDER, FREE_CHAIRS, DOOR_CAN_BE_USED};

enum who{BARBER, CLIENT};

typedef struct haircut{
    int length;
} haircut;

typedef struct barbershop{
    int next_person_id;
    haircut hair; 
    int last_taken_chair;
    int size;
} barbershop;

void print_info(enum who is_client);

#if defined(DEBUG) && DEBUG > 1
void print_shm(barbershop* shop, pid_t* queue);
void print_sem(sem_t *sem[]);
#endif //DEBUG

#ifdef OUTPUT
extern FILE* file;
void create_log_file(pid_t);
#endif //OUTPUT

#endif //CONST

