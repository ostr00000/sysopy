#define _XOPEN_SOURCE
#include <stdlib.h> //getenv
#include "const.h"
#include <sys/ipc.h>//ftok
#include <sys/msg.h>//msgget, msgrcv, msgsnd
#include <unistd.h> //sleep, getpid
#include <time.h> //time, localtime, struct tm, strftime
#include <ctype.h>  //toupper

int main(){
    printf("server started\n");

    char* home=getenv("HOME");
    key_t key;
    IS_ERROR((key=ftok(home,SERVER_VAR)),"server key generated\n");
    int queue=msgget(key,IPC_CREAT|0666);
    IS_ERROR(queue,"queue created\n");


    const int pid=getpid();
    const int message_size=sizeof(message)-sizeof(long);
    message ms;
    int client,msgtyp=0,msgflg=0;
    

    int clients_connected=0;
    struct{
        pid_t pid;
        int queue_id;
    }clients[MAX_CLIENTS];    
    

    while(1){
        /*if 5th arg is equal to 0 -> oldest message is received*/
        /*if 5th arg is less than 0 -> less or equal to the abs val of msgtpe is recieved*/
        int ret=msgrcv(queue,&ms,message_size,msgtyp,msgflg);
        if(-1==ret && ENOMSG==errno){   /*if recieved EXIT and no message -> exit*/
            IS_ERROR(msgctl(queue,IPC_RMID,0),"server queue deleted\n");
            return 0;
        }
        IS_ERROR(ret,"recieved msg\n");

        switch(ms.type){
            case FIRST_CONTACT:{
                if(clients_connected==MAX_CLIENTS){
                    printf("cannot conext next client - max number of client achieved\n");
                    continue;
                }
                int id;
                IS_ERROR((id=msgget(ms.data.key,0666)),"client queue has been opened\n");
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
                msgtyp=(-1)*EXIT;
                msgflg=IPC_NOWAIT;
                continue;
            }
        }

        client=-1;
        for(int i=0;i<clients_connected;i++){
            if(clients[i].pid==ms.pid){
                client=clients[i].queue_id;
                break;            
            }
        }


        ms.pid=pid;
        IS_ERROR(msgsnd(client,&ms,message_size,0),"server send msg to client\n");
        sleep(1);
    }    
    
    return 0;
}

