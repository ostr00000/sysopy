#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

#ifndef MAX_SLEEP_TIME
#define MAX_SLEEP_TIME (20)
#endif


int requests=0,N,K;
int pos_end,pos_start=0;
pid_t* childs;
pid_t* child_requests;
pid_t parent;

void send_SIGUSR2(pid_t pid){
    #ifdef debug
    fprintf(stderr, "sending sig to process %d\n",pid);
    #endif // debug

    kill(pid,SIGUSR2);
}


int child_get_response=0;
void recieve_request(int sig_number,siginfo_t* siginfo,void* uncontext ){
	fprintf(
         stderr, "process PID=%d sent signal %s to proces PID=%d\n",
         siginfo->si_pid, strsignal(siginfo->si_signo), getpid()
    );
	if(sig_number==SIGUSR1){
        if(requests<K){
            child_requests[requests++]=siginfo->si_pid;
            if(requests==K){
                #ifdef debug
                fprintf(stderr, "limit achieved\n");
                #endif // debug

                for(int i=0;i<requests;i++){
                    send_SIGUSR2(child_requests[i]);
                }
            }
        }else{
            send_SIGUSR2(siginfo->si_pid);
        }
	}else if(sig_number==SIGUSR2){
        child_get_response=1;
    }
}


void end(){
    if(getpid()==parent){
        for(int i=pos_start;i<pos_end;i++){
            kill(childs[i],SIGKILL);
        }
    }

    #ifdef debug
    fprintf(stderr,"\nending process %d\n",getpid());
    #endif // debug

    exit(3);
}


int main(int argc,char* argv[]){
	if(argc<3){
		printf("give 2 argumnets: N_processes K_requests \n");
		return 1;
	}
	N=strtol(argv[1],NULL,10);
	K=strtol(argv[2],NULL,10);

	child_requests=calloc(N,sizeof(pid_t));
    childs=calloc(N,sizeof(pid_t));

	struct sigaction act;
	act.sa_sigaction = recieve_request;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &act, NULL);
	sigaction(SIGUSR2, &act, NULL);

	for(int i=0;i<32;i++){
        sigaction(SIGRTMIN+i,&act,NULL);
	}

    parent=getpid();
	signal(SIGINT,end);

	for(pos_end=0;pos_end<N;pos_end++){
		if( (childs[pos_end]=fork())<0){
            perror("fork error\n");
            return 2;
        }else if(childs[pos_end]==0){
            srand(time(NULL)*getpid());
            time_t send_request,get_response;
            int ran=rand()%11;

            #ifdef debug
            pid_t my_pid=getpid();
            fprintf(stderr, "process PID=%d simulating work %d\n",my_pid,ran);
            #endif // debug

            sleep(ran);

            #ifdef debug
            fprintf(stderr, "process PID=%d send request\n",my_pid);
            #endif // debug

            time(&send_request);
			kill(getppid(),SIGUSR1);

			#ifdef debug
            fprintf(stderr, "process PID=%d waiting\n",my_pid);
            #endif // debug

			sleep(MAX_SLEEP_TIME);
            if(child_get_response!=1){
                return 255;
            }

			#ifdef debug
			fprintf(stderr, "process PID=%d continue\n",my_pid);
            #endif // debug

			time(&get_response);
			kill(getppid(),SIGRTMIN+rand()%32);
			return get_response-send_request;
        }else{
            fprintf(stderr,"%d process\n",pos_end);
        }
	}
	
    int too_long_waiting=0;
    int status;
	for(pos_start=0;pos_start<N;pos_start++){
        if(waitpid(childs[pos_start],&status,0)==-1){
            #ifdef debug
            fprintf(stderr,"waiting for child PID=%d failed\n",childs[pos_start]);
            #endif // debug

            pos_start--;
            continue;
        }
        if(WIFEXITED(status)){
            int ret=WEXITSTATUS(status);
            if(ret==255){
                too_long_waiting++;
            }
            fprintf(stderr,"PID %d, retuned %d\n",childs[pos_start],ret);
        }else if(WIFSIGNALED(status)){
            fprintf(stderr,"error: PID %d terminated with signal %s\n",childs[pos_start],strsignal(WTERMSIG(status)));
        }
	}

    #ifdef debug
    printf("the program has been end, there were %d process that lost signal\n",too_long_waiting);
    #endif // debug	

	return 0;
}
