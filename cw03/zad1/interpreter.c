#define _XOPEN_SOURCE 600

#include <unistd.h> /* fork*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>/*perror*/
#include <sys/types.h> /*pid_t*/
#include <errno.h>/*errno*/
#include <wait.h> /*wait*/

#define BUF_MAX_SIZE (200)
#define ARG_LIMIT (10)

void parse(char* line,char* tab[]){
    int i,args_num=1,space=0,apostr=0;
    tab[0]=line;
    for(i=1;i<ARG_LIMIT+1;i++)tab[i]=NULL;

    for(i=1;i<BUF_MAX_SIZE-1 && line[i]!='\0' && line[i]!='\n';i++){
        if(line[i]==' ' && !apostr){
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
}

int main(int argc,char* argv[]){
    if(argc!=2){
        perror("Get 1 argument: fileName\n");
        return 1;
    }
    FILE* file=fopen(argv[1],"r");
    if(file==NULL){
        char* e=malloc(BUF_MAX_SIZE);
        snprintf(e,BUF_MAX_SIZE,"cannot open file: %s\n",argv[1]);
        perror(e);
        return 2;
    }
    char* line=(char*)malloc(BUF_MAX_SIZE);
    char* name=(char*)malloc(BUF_MAX_SIZE);
    char* value=(char*)malloc(BUF_MAX_SIZE);

    while(NULL!=fgets(line,BUF_MAX_SIZE,file)){
        int readed;
        sscanf(line,"\n%n",&readed);
        if(readed==1)continue;

        readed=sscanf(line,"#%s %s",name,value);
        if(readed==2){
            setenv(name,value,1);
            continue;
        }

        readed=sscanf(line,"#%s",name);
        if(readed==1){
            unsetenv(name);
            continue;
        }

        char* tab[ARG_LIMIT+1];
        parse(line,tab);
        pid_t pid;
        if( (pid=fork())<0){
            perror("fork error\n");
        }else if(pid==0){
            execvp(tab[0],tab);
        }else{
            int status;
            if(wait(&status)!=pid){
                perror("wait error\n");
                return 6;
            }
            if(status){
                printf("Program %s has been returned %d\n",tab[0],status);
                return 0;
            }
        }
    }
    if(ferror(file)){
        perror(strerror(errno));
        return 3;
    }
    fclose(file);
    return 0;
}
