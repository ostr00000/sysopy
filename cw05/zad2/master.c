#define _XOPEN_SOURCE 500
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
int R;

int main(int argc,char* argv[]){
    if(argc!=3){
        printf("give 2 args: path_to_file R_image_size \n");
        return 1;
    }
    R=strtol(argv[2],NULL,10);
    const char* file_path=argv[1];    
    
    mkfifo(file_path, 777);
    FILE* fifo=fopen(file_path,"r");

    int** T=malloc(R*sizeof(int*));
    for(int i=0;i<R;i++){
        T[i]=calloc(R,sizeof(int));
    }
    int q=0;

    printf("initialisation succesfull\n");    
    fflush(stdout);

    while(1){
        q++;
        double x,y;
        int val;        
        int ret=fscanf(fifo,"%lf %lf %d\n",&x,&y,&val);
        if(EOF==ret || ret==0)break;        
        
        if(q%100000==0){
            printf("loaded %d data\n",q);
            fflush(stdout);
        }
        x=(x+2)*R/3;
        y=(y+1)*R/2;
        //printf("ret=%d x=%lf, y=%lf\n",ret,x,y);
        if(x<R && x>=0 && y<R && y>=0){
            T[(int)x][(int)y]=val;
        }else{
            printf("round error: x=%d, y=%d\n",(int)x,(int)y);
        }
    }

    printf("loop ended succesfull\n");

    FILE* data=fopen("data","w");
    char* buf=malloc(100);
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){       
            sprintf(buf,"%d %d %d\n",i,j,T[i][j]);
            fwrite(buf,1,strlen(buf),data);
        }   
    }
    fclose(data);

    printf("write to file ended succesfull\n");

    FILE* gnuplot=popen("gnuplot","w");
    fputs("set view map\n",gnuplot);
    sprintf(buf,"set xrange [0:%d]\n",R);
    fputs(buf,gnuplot);
    sprintf(buf,"set yrange [0:%d]\n",R);
    fputs(buf,gnuplot);
    fputs("plot 'data' with image\n",gnuplot);
    
    fflush(gnuplot);
    getc(stdin);
}
