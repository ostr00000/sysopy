#define _XOPEN_SOURCE
#include <stdlib.h> //getenv,rand,srand,
#include "const.h"
#include <sys/ipc.h>//ftok
#include <sys/msg.h>//msgget, msgrcv, msgsnd
#include <unistd.h> //time
#include <time.h> //time

#define TEXT_EXAMPLE_SIZE 15
const char text_example[TEXT_EXAMPLE_SIZE][MAX_MESSAGE_SIZE];

int main(){
    printf("client started\n");


    char* home=getenv("HOME");
    key_t key;
    IS_ERROR((key=ftok(home,SERVER_VAR)),"server key generated\n");
    int server=msgget(key,0666);
    IS_ERROR(server,"server queue opened\n");


    int client;
    for(int i=0;i<MAX_CLIENTS;i++){
        IS_ERROR((key=ftok(home,SERVER_VAR+i)),"client key generated\n");
        client=msgget(key,0666|IPC_CREAT|IPC_EXCL);
        if(-1!=client)break;
    }
    IS_ERROR(client,"client queue created\n");

    srand(time(NULL));
    const int pid=getpid();
    const int message_size=sizeof(message)-sizeof(long);
    message ms;
    ms.type=FIRST_CONTACT;
    ms.pid=pid;
    ms.data.key=key;
    IS_ERROR(msgsnd(server,&ms,message_size,0),"client snd first msg to server\n");
    
    /*if 5th arg is greater than 0 -> message of type msgtyp is received*/
    IS_ERROR(msgrcv(client,&ms,message_size,FIRST_CONTACT,0),"client recieved first msg from server\n");
    
    while(1){
        int random=rand()%100;
        if(random<70){
            ms.type=(random<40)?ECHO:WERS;
            memcpy(ms.data.text,text_example[random%TEXT_EXAMPLE_SIZE],MAX_MESSAGE_SIZE);
            printf("[CLIENT SEND]: %s\n",ms.data.text);
        }else if(random<95){
            printf("[CLIENT ASK FOR TIME]\n");
            ms.type=TIME;
        }else{
            printf("[CLIENT ORDER TO END]\n");
            ms.type=EXIT;
            IS_ERROR(msgctl(client,IPC_RMID,0),"client queue deleted\n");
        }
        
        ms.pid=pid;
        IS_ERROR(msgsnd(server,&ms,message_size,0),"send msg to server\n");
        if(ms.type==EXIT){
            return 0;      
        }        
        /*if 5th arg is equal to 0 -> oldest message is received*/
        IS_ERROR(msgrcv(client,&ms,message_size,0,0),"recieved server msg\n");
        printf("[SERVER]: %s\n",ms.data.text);
    }
    return 0;
}

const char text_example[TEXT_EXAMPLE_SIZE][MAX_MESSAGE_SIZE]={
    "Sometimes, all you need to do.","Last Friday in three week’s time I saw a spotted striped blue worm shake hands with a legless lizard.","My Mum tries to be cool by saying that she likes all the same things that I do.","He told us a very exciting adventure story.","I am counting my calories, yet I really want dessert.","She did not cheat on the test, for it was not the right thing to do.","This is a Japanese doll.","A purple pig and a green donkey flew a kite in the middle of the night and ended up sunburnt.","This is the last random sentence I will be writing and I am going to stop mid-sent","I think I will buy the red car, or I will lease the blue one.","I checked to make sure that he was still alive.","The book is in front of the table.","I am happy to take your donation; any amount will be greatly appreciated.","If the Easter Bunny and the Tooth Fairy had babies would they take your teeth and leave chocolate for you?","Sixty-Four comes asking for bread."};

