#ifndef CONST
#define CONST

#define _XOPEN_SOURCE
#include <sys/types.h> //pid_t,time_t
#include <string.h> //strerror
#include <errno.h>  //errno
#include <stdio.h>  //printf

#define SERVER_VAR 1
#define MAX_MESSAGE_SIZE 128
#define MAX_CLIENTS 20

#ifdef DEBUG
#define IS_ERROR(fun,str_suc)\
    do{ if(-1==(fun)){\
        perror(strerror(errno));\
        return 1;\
    }else{\
        printf(str_suc);\
    }}while(0)
#else 
#define IS_ERROR(fun,str_suc)\
    do{ if(-1==(fun)){\
        perror(strerror(errno));\
        return 1;\
    }}while(0)
#endif //DEBUG
    


typedef struct{
    long type;
    pid_t pid;
    union {
        char text[MAX_MESSAGE_SIZE];
        key_t key;
        int client_id;
    }data;
}message;

enum msg_type {FIRST_CONTACT=1,ECHO,WERS,TIME,EXIT};


#endif //CONST
