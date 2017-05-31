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
#include <sys/ipc.h>    //ftok
#include <string.h> //strerror
#include <errno.h>  //errno
#include <stdio.h>  //printf
#include <stdlib.h> //strtol
#include <sys/sem.h>    //semget
#include <sys/shm.h>    //shmget
#include <time.h>   //clock_gettime
#include <signal.h> //signal
#include <unistd.h> //getpid
#include <sys/wait.h>   //wait

#ifndef MAX_TIME_SLEEP 
#define MAX_TIME_SLEEP 5 /* when client see full queue they go to sleep */
#endif

#define BARBER_SEM_KEY (ftok("/barber",'a'))
#define BARBER_SHM_KEY (ftok("/barber",'b'))


#if defined(DEBUG) && DEBUG != 2
#define IS_ERROR3(left, right, str_suc)\
    do{ if(left == right){\
        fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, strerror(errno));\
        exit(1);\
    }else{\
        fprintf(stderr, "%s", str_suc);\
    }}while(0)

#define AT_EXIT(fun)\
    do{ if(0 != atexit(fun)){\
        fprintf(stderr, "%s:%d: cannot set atexit function\n", __func__, __LINE__);\
        (fun)();\
        exit(1);\
    }else{\
        fprintf(stderr, "function atexit set correctly\n");\
    }}while(0)

#else
#define IS_ERROR3(left, right, str_suc)\
    do{ if(left == right){\
        fprintf(stderr, "%s\n", strerror(errno));\
        exit(1);\
    }}while(0)

#define AT_EXIT(fun)\
    do{ if(0 != atexit(fun)){\
        fprintf(stderr, "cannot set atexit function\n");\
        (fun)();\
        exit(1);\
    }}while(0)

#endif //DEBUG

#define IS_ERROR(fun, str_suc) IS_ERROR3(-1, fun, str_suc)

#if defined(DEBUG) && DEBUG != 2
#define IS_ERROR_VAL(fun, str_suc, val)\
    do{ IS_ERROR(fun, str_suc);\
        fprintf(stderr, "%d\n", val);\
    }while(0)
#else
#define IS_ERROR_VAL(fun, str_suc, val) IS_ERROR(fun, str_suc)
#endif //DEBUG


enum semaphore{BARBER_IS_SLEEPING, FREE_CHAIRS, DOOR_CAN_BE_USED,
               NEXT_CUST1, NEXT_CUST2, CLIENT_READY, PAY_BARBER, ANULUJ};

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
void wait_for_semaphore(int sem, enum semaphore number, char string[]);
void change_semaphore(int sem, enum semaphore number, char string[], int val);

#if defined(DEBUG) && DEBUG > 1
void print_shm(barbershop* shop, pid_t* queue);
void print_sem(int sem);
#endif //DEBUG

#ifdef OUTPUT
extern FILE* file;
void create_log_file(pid_t);
#endif //OUTPUT

#endif //CONST

