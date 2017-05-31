#ifndef CONST
#define CONST

#define _XOPEN_SOURCE 600
#include <sys/types.h> //pid_t,time_t
#include <string.h> //strerror, memcpy, strcpy
#include <errno.h>  //errno
#include <stdio.h>  //printf
#include <limits.h> //NAME_MAX

#define SERVER_NAME "/server"
#define MAX_MESSAGE_SIZE (128)
#define MAX_CLIENTS (20)  

#ifdef DEBUG
#define IS_ERROR(val,fun,str_suc)\
    do{ if((val)==(fun)){\
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(strerror(errno));\
            return 1;\
        }else{\
            printf(str_suc);\
        }}while(0)
#else 
#define IS_ERROR(val,fun,str_suc)\
    do{ if((val)==(fun)){\
            perror(strerror(errno));\
            return 1;\
    }}while(0)
#endif //DEBUG
    
typedef enum {FIRST_CONTACT=1,ECHO,WERS,TIME,EXIT} msg_type;

typedef struct{
    msg_type type;
    pid_t pid;
    union {
        char text[MAX_MESSAGE_SIZE];
        char key[MAX_MESSAGE_SIZE];
        int client_id;
    }data;
}message;


#endif //CONST
