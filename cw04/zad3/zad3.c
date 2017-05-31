#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

struct proces{
    pid_t pid;
    int live;
};

int N,L,sleep_time=0;
struct proces* childs;
int num_of_signals=0;

void end(int sig){
    for(int i=0;i<N;i++){
        if(childs[i].live){
            kill(childs[i].pid,SIGKILL);
        }
    }
    exit(4);
}


void add_signal(int sig_number,siginfo_t* siginfo,void* uncontext ){
    if(sig_number==SIGUSR2){
        fprintf(stderr,"child PID=%d recieved %d SIGUSR1 signals\n",getpid(),num_of_signals);
        exit(0);
    }
    num_of_signals++;
    kill(getppid(),SIGUSR1);
}

int resend_signals;
void resended(int sig_number,siginfo_t* siginfo,void* uncontext ){
    resend_signals++;
}

int main(int argc,char* argv[]){
    srand(time(NULL));
    if(argc<3){
        printf("get arguments: N_num_of_child_process L_num_of_signals [sleep_time]\n");
        return 1;
    }
    N=strtol(argv[1],NULL,10);
    L=strtol(argv[2],NULL,10);
    if(argc==4)sleep_time=strtol(argv[3],NULL,10);
    if(N<1 || sleep_time<0 || L<1){
        printf("invalid arguments\n");
        return 2;
    }
    fprintf(stderr,"%s N=%d L=%d T=%d\n",argv[0],N,L,sleep_time);
    childs=calloc(N,sizeof(struct proces));
    for(int i=0;i<N;i++)childs[i].live=0;
    signal(SIGINT,end);

    struct sigaction act;
    act.sa_sigaction = resended;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    for(int i=0;i<N;i++){
        childs[i].live=1;
        if((childs[i].pid=fork())<0){
            perror("fork error\n");
            return 3;
        }else if(childs[i].pid==0){
            act.sa_sigaction = add_signal;
            sigemptyset(&act.sa_mask);
            sigaddset(&act.sa_mask,SIGUSR1);
            act.sa_flags = SA_SIGINFO;

            sigaction(SIGUSR1, &act, NULL);
            sigaction(SIGUSR2, &act, NULL);

            while(1)sleep(10);
        }
    }

    int living_process=N,i,fun;
    while(living_process){
        while(1){
            i=rand()%N;
            if(childs[i].live)break;
        }

        living_process--;
        fun=rand()%3;
        resend_signals=0;
        fprintf(stderr,"\tsending to child PID=%d: ",childs[i].pid);

        if(fun==0){
            fprintf(stderr,"%d signals using \"kill\"\n",L);
            for(int j=0;j<L;j++)kill(childs[i].pid,SIGUSR1);
            kill(childs[i].pid,SIGUSR2);

        }else if(fun==1){
            fprintf(stderr,"%d signals using \"sigqueue\"\n",L);
            union sigval value;
            for(int j=0;j<L;j++)sigqueue(childs[i].pid,SIGUSR1,value);
            sigqueue(childs[i].pid,SIGUSR2,value);

        }else {
            fprintf(stderr,"SIGRTMIN SIGRTMAX\n");
            kill(childs[i].pid,SIGRTMIN);
            kill(childs[i].pid,SIGRTMAX);
        }
        sleep(sleep_time);
        fprintf(stderr,"parent recieved %d SIGUSR1 signals from %d\n\n",resend_signals,childs[i].pid);
        childs[i].live=0;
    }

    return 0;
}
