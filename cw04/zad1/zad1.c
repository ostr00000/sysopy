
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

char znak='a';
int rosnaco=1;

void zmien(){
        rosnaco=(rosnaco)?0:1;
}
void end(){
        printf("\tOdebrano sygnał SIGINT\n");
        exit(0);
}

int main(){
        struct sigaction act;
        act.sa_handler=zmien;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGTSTP, &act, NULL);

        signal(SIGINT,end);
        while(1){
                if(rosnaco && znak=='z'){
                        znak='a';
                }else if(!rosnaco && znak=='a'){
                        znak='z';
                }
                printf("%c\n",(rosnaco)?znak++:znak--);
                usleep(300000);

        }

        return 0;
}
