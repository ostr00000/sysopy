/* int must have size of 4 bytes */
#ifndef CONST
#define CONST

#if !defined(TCP) && !defined(UDP)
#define TCP //if no flag setted, then set TCP
#elif defined(TCP) && defined(UDP)
#error There must be only one of flags setted (TCP | UDP)
#endif


#if !defined(PROTOCOL) && !defined(TYP)

#if   defined(TCP)
#define PROTOCOL "TCP"
#define TYP SOCK_STREAM
#elif defined(UDP)
#define PROTOCOL "UDP"
#define TYP SOCK_DGRAM
#endif

#else //PROTOCOL
#error Falg PROTOCOL and TYP is used by program - remove it
#endif //PROTOCOL

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <endian.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define COMMA ,

#ifdef DEBUG
#define IS_ERROR(fun, print)\
    do{ if(-1 == (fun)){\
            char c[10];\
            fprintf(stderr, "%s:%d: %s\n", __func__, __LINE__, strerror_r(errno, &c[0], 10));\
            exit(EXIT_FAILURE);\
        }else{\
            fprintf(stderr, print);\
    }}while(0)
#else
#define IS_ERROR(fun, print)\
    do{ if(-1 == (fun)){\
            char c[10];\
            fprintf(stderr, "%s\n", strerror_r(errno, &c[0], 10));\
            exit(EXIT_FAILURE);\
    }}while(0)
#endif

/* char is index in array - MAX_CLIENT can't be greater than 127 */
#define MAX_CLIENTS (16)
#define CLIENT_MAX_NAME (40)
#define SLEEP_TIME (10)
#define MAX_PATH_SIZE (sizeof((((struct sockaddr_un *) 0)->sun_path)))

enum connection_type{LOCAL, NETWORK};

enum message_type{
    REGISTER,
    COMPUTED_RESULT,
    UNREGISTER,
    CLIENT_OK,
    CLIENT_NAME_USED,
    NO_FREE_SLOT,
    CONTROL,
    ORDER
}__attribute__((packed));

typedef struct{
    char op;
    int numA;
    int numB;
    int exp_id;
}__attribute__((packed)) expression;

typedef struct{
    int result;
    int exp_id;
}expression_result; 

#ifdef TCP
typedef struct{
    char* name;
    char id;
}client_info;

#else //UDP
typedef struct{
    enum connection_type type;
    struct sockaddr_storage address;
    socklen_t address_len;
    char* name;
}client_info;

typedef struct {
    enum message_type type;
    union{
        char name[CLIENT_MAX_NAME];
        expression exp;
        expression_result result;
    }data;
}message_struct;
#endif //TCP, UDP

int send_all(int sockfd, const void *msg, int len, int flags, struct sockaddr *to, socklen_t tolen);
int receive_all(int sockfd, void *msg, int len, int flags, struct sockaddr *from, socklen_t *fromlen);

#endif //CONST
