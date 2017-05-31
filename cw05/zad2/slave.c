#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

char* path_to_file;
int N,K;

int main(int argc,char* argv[]){
    if(argc!=4){
        printf("give 3 args: path_to_file N_random_points K_iteration \n");
    }
    N=strtol(argv[2],NULL,10);    
    K=strtol(argv[3],NULL,10);
    path_to_file=argv[1];
    srand(time(NULL));
    int fd = open(path_to_file, O_WRONLY);
    char buf[100];    
    /* D = [-2, 1] x [-1, 1] */
    for(int i=0;i<N;i++){
        double re=3*(double)rand()/((double)RAND_MAX)-2;
        double im=2*(double)rand()/((double)RAND_MAX)-1;
        double complex point=re+im*I;
        double complex z=0;
        int val=0;
        do{
            ++val;
            z=cpow(z,2)+point;
        }while(cabs(z)<2 && val<K);
        sprintf(buf,"%lf %lf %d\n",re,im,val);
        write(fd,buf,strlen(buf));
        //printf("%s\n",buf);
    }
}
