#include "const.h"
#include <unistd.h> //getpid
#include <time.h>   //time
#include <mqueue.h> //mqd_t
#include <stdlib.h> //rand,srand

#define TEXT_EXAMPLE_SIZE 15
const char text_example[TEXT_EXAMPLE_SIZE][MAX_MESSAGE_SIZE];

int main(){
    printf("client started\n");
    const int pid=getpid();
    const int message_size=sizeof(message);
    srand(time(NULL));

    mqd_t server=mq_open(SERVER_NAME,O_WRONLY);
    IS_ERROR((mqd_t)-1,server,"server queue opened\n");

    const int max=(MAX_MESSAGE_SIZE<NAME_MAX)?MAX_MESSAGE_SIZE:NAME_MAX;
    char key[NAME_MAX];
    key[0]='/';
    for(int i=1;i<max-1;i++){
        key[i]=rand()%25+65;/* key <- "/[A-Z]{max}" */
    }
    key[max-1]='\0';
    
    struct mq_attr attr;
    attr.mq_maxmsg = 2;
    attr.mq_msgsize = message_size;
    mqd_t client=mq_open(key,O_RDONLY|O_CREAT,0666,&attr);
    IS_ERROR((mqd_t)-1,client,"client queue created\n");
    

    message ms;
    ms.type=FIRST_CONTACT;
    ms.pid=pid;
    strcpy(ms.data.key,key);
    IS_ERROR(-1,mq_send(server,(char*)&ms,message_size,0),
             "client snd first msg to server\n");
    IS_ERROR(-1,mq_receive(client,(char*)&ms,message_size,0),
             "client recieved first msg from server\n");
    
    while(1){
        int random=rand()%100;
        if(random<70){
            ms.type=(random<40)?ECHO:WERS;
            memcpy(ms.data.text,text_example[random%TEXT_EXAMPLE_SIZE],MAX_MESSAGE_SIZE);
            printf("[CLIENT SEND %s]: %s\n",(random<40)?"ECHO":"WERS",ms.data.text);
        }else if(random<95){
            printf("[CLIENT ASK FOR TIME]\n");
            ms.type=TIME;
        }else{
            printf("[CLIENT ORDER TO END]\n");
            ms.type=EXIT;
            IS_ERROR(-1,mq_close(client),"client queue closed\n");
            IS_ERROR(-1,mq_unlink(key),"client queue deleted\n");
        }
        
        ms.pid=pid;
        IS_ERROR(-1,mq_send(server,(char*)&ms,message_size,0),"send msg to server\n");
        if(ms.type==EXIT){
            return 0;      
        }        
        IS_ERROR(-1,mq_receive(client,(char*)&ms,message_size,0),"recieved server msg\n");
        printf("[SERVER RESPONSE ]: %s\n",ms.data.text);
    }
}

const char text_example[TEXT_EXAMPLE_SIZE][MAX_MESSAGE_SIZE]={
    "Sometimes, all you need to do.","Last Friday in three week’s time I saw a spotted striped blue worm shake hands with a legless lizard.","My Mum tries to be cool by saying that she likes all the same things that I do.","He told us a very exciting adventure story.","I am counting my calories, yet I really want dessert.","She did not cheat on the test, for it was not the right thing to do.","This is a Japanese doll.","A purple pig and a green donkey flew a kite in the middle of the night and ended up sunburnt.","This is the last random sentence I will be writing and I am going to stop mid-sent","I think I will buy the red car, or I will lease the blue one.","I checked to make sure that he was still alive.","The book is in front of the table.","I am happy to take your donation; any amount will be greatly appreciated.","If the Easter Bunny and the Tooth Fairy had babies would they take your teeth and leave chocolate for you?","Sixty-Four comes asking for bread."};

