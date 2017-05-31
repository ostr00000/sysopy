#include "const.h"
#include <unistd.h> //getpid, sleep
#include <time.h>   //time, localtime, struct tm, strftime
#include <mqueue.h> //mqd_t
#include <ctype.h>  //toupper
#include <fcntl.h>  //O_CREAT

int clients_connected=0;
struct{
    pid_t pid;
    mqd_t queue_id;
}clients[MAX_CLIENTS];

mqd_t find_mqd(pid_t pid){
    for(int i=0;i<clients_connected;i++){
        if(clients[i].pid==pid){
            return clients[i].queue_id;   
        }
    }
    return -1;
}

int main(){
    printf("server started\n");
    const size_t message_size=sizeof(message);
    const int pid=getpid();

    struct mq_attr attr;
    attr.mq_flags=0;
    attr.mq_maxmsg=10;
    attr.mq_msgsize=message_size;
    attr.mq_curmsgs=0;

    mqd_t client,queue=mq_open(SERVER_NAME, O_RDONLY | O_CREAT| O_EXCL, 0666, &attr);
    if((mqd_t)-1==queue && EEXIST==errno){
        IS_ERROR(-1,mq_unlink(SERVER_NAME),"old queue deleted\n");
        queue=mq_open(SERVER_NAME, O_RDONLY | O_CREAT, 0666, &attr);
    }
    IS_ERROR((mqd_t)-1,queue,"queue created\n");

    message ms;
    while(1){
        sleep(1);
        ssize_t ret=mq_receive(queue,(char*)&ms,message_size,NULL);
        if(-1==ret && EAGAIN==errno){
            IS_ERROR(-1,mq_close(queue),"server queue closed\n");
            IS_ERROR(-1,mq_unlink(SERVER_NAME),"server queue deleted\n");
            return 0;
        }
        IS_ERROR(-1,ret,"recieved msg\n");

        switch(ms.type){
            case FIRST_CONTACT:{
                if(clients_connected==MAX_CLIENTS){
                    printf("cannot conext next client - max number of client achieved\n");
                    continue;
                }
                mqd_t id;
                #ifdef DEBUG 
                printf("client name:%s\n",ms.data.key);
                #endif //DEBUG
                IS_ERROR((mqd_t)-1,(id=mq_open(ms.data.key,O_WRONLY)),
                         "client queue has been opened\n");
                clients[clients_connected].pid=ms.pid;
                clients[clients_connected].queue_id=id;
                ms.data.client_id=clients_connected++;
                break;
            }
            case ECHO:{
                break;
            }
            case WERS:{
                for(int i=0;i<MAX_MESSAGE_SIZE;i++){
                    ms.data.text[i]=toupper(ms.data.text[i]);
                }
                break;
            }
            case TIME:{
                time_t current_time;
                time(&current_time);                
                struct tm* time_info=localtime(&current_time);
                strftime(ms.data.text,MAX_MESSAGE_SIZE-1,"%F %T",time_info);
                break;
            }       
            case EXIT:{
                IS_ERROR(-1,mq_close(find_mqd(ms.pid)),"client queue closed\n");
                attr.mq_flags=O_NONBLOCK;
                IS_ERROR(-1,mq_setattr(queue,&attr,NULL),"server parameters changed\n");
                attr.mq_flags=0;
                continue;
            }
        }

        client=find_mqd(ms.pid);
        ms.pid=pid;
        IS_ERROR(-1,mq_send(client,(char*)&ms,message_size,0),"server send msg to client\n");
    }    
    
    return 0;
}

