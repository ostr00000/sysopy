#include "const.h"

int send_all(int sockfd, const void *msg, int len, int flags,
             struct sockaddr *to, socklen_t tolen){
    #ifdef DEBUG
    fprintf(stderr, "control print sender: sockfd=%d, msg_type=%u len=%d, flags=%d\n", sockfd,
            #ifdef TCP
            (unsigned)*msg,
            #else //UDP
            (unsigned) ((message_struct *)msg)->type,
            #endif //TCP, UDP
            len, flags);
    #endif

    char* ptr = (char*) msg;
    while(len > 0){
        int res = sendto(sockfd, ptr, len, flags, to, tolen);
        len -= res;
        ptr += res;
        IS_ERROR(res, "message sended, remain: %d bytes\n" COMMA len);
    }
    return 1;
}

int receive_all(int sockfd, void *msg, int len, int flags,
                struct sockaddr *from, socklen_t *fromlen){
    #ifdef DEBUG
    fprintf(stderr, "control print reciever: sockfd=%d, len=%d, flags=%d\n",
            sockfd, len, flags);
    #endif

    char* ptr = (char*) msg;
    while(len > 0){
        int res = recvfrom(sockfd, ptr, len, flags, from, fromlen);
        len -= res;
        ptr += res;
        IS_ERROR(res, "message recieved, remain: %d bytes\n" COMMA len);
        if(res == 0){
            return 0;
        }
    }
    return 1;
}

