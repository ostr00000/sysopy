//#define _XOPEN_SOURCE  700
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<string.h>


#ifndef ARG_LIMIT
#define ARG_LIMIT (10)
#endif

#ifndef COMMAND_LIMIT
#define COMMAND_LIMIT (20)
#endif

#ifndef BUF_MAX_SIZE
#define BUF_MAX_SIZE (1000)
#endif

int parse(char* line,char* tab[],int last_parsed_char){
    int i,args_num=0,space=1,apostr=0;
    /*while(line[last_parsed_char]==' ' || line[last_parsed_char]=='\"'){
        last_parsed_char++;
    }*/
    //tab[0]=&line[last_parsed_char];
    for(i=1;i<ARG_LIMIT+1;i++)tab[i]=NULL;

    for(i=last_parsed_char;i<BUF_MAX_SIZE-1 && line[i]!='\0' && line[i]!='\n';i++){
        if(line[i]=='|'){
            line[i]='\0';
            return i+1;
        }else if(line[i]==' ' && !apostr){
            line[i]='\0';
            space=1;
        }else if(line[i]=='\"'){
            apostr=(apostr)?0:1;
            line[i]='\0';
        }else if(space){
            space=0;
            if(args_num==ARG_LIMIT)break;
            tab[args_num++]=&line[i];
        }
    }
    line[i]='\0';
    if(tab[0][0]=='\"' || tab[0][0]=='\n' || tab[0][0]=='\0')return -2;
    return -1;
}

void swap(int l[2],int r[2]){
    for(int i=0,tmp;i<2;i++){
        tmp=l[i];
        l[i]=r[i];
        r[i]=tmp;
    }
}

int main(void){
    char buf[BUF_MAX_SIZE];
    char* tab[ARG_LIMIT+1];

    pid_t childs[COMMAND_LIMIT];
    int last[2];
    int now[2];
    swap(last,now);
    int q=1;
    while(q){

        if(NULL==fgets(buf,BUF_MAX_SIZE,stdin)){
            printf("read error\n");
            return 1;
        }//*/
        //strcpy(buf,argv[q++]);
        int num_command;
        pipe(now);
        close(now[1]);
        int last_parsed_char=0;
        for(num_command=0;num_command<COMMAND_LIMIT && last_parsed_char!=-1;num_command++){
            last_parsed_char=parse(buf,tab,last_parsed_char);
            if(last_parsed_char==-2)break;
            swap(last,now);
            pipe(now);
            if((childs[num_command]=fork())<0){
                printf("fork error\n");
            }else if(childs[num_command]==0){
                dup2(now[1],STDOUT_FILENO);
                dup2(last[0],STDIN_FILENO);
                close(last[1]);
                close(now[0]);
                execvp(tab[0],tab);
            }
            close(last[0]);
            close(now[1]);
        }

        for(int i=0;i<num_command;i++){
            int status;
            waitpid(childs[i],&status,0);
            if(WIFEXITED(status)){
                fprintf(stdout,"process PID=%d exited with status %d\n",childs[i],WEXITSTATUS(status));
            }else if(WIFSIGNALED(status)){
                fprintf(stderr,"error: process PID=%d terminated %d\n",childs[i],WTERMSIG(status));
                return 2;
            }
        }
        int r=0;
        while((r=read(now[0],buf,BUF_MAX_SIZE))){
            write(STDOUT_FILENO,buf,r);
        }
        close(now[0]);
    }

	return 0;
}
