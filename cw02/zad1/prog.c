#include <stdlib.h>
#include <stdio.h>//printf
#include <string.h>//strcmp
#include <fcntl.h>//funkcje systemowe
#include <unistd.h> //funkcje read, write, close
#include <sys/stat.h> //S_IRUSR ...
#include <errno.h>  //wypisanie bledu
#include <time.h> //rand

int sys0lib1,gen0shu1sort2,rekordy,wielkosc_rekordow;
const char* filename;

void arg(){
    printf("podaj argumenty: {sys,lib} {generate,shuffle,sort} nazwaPliku liczbaRekordow wilekoscRekordow\n");
}
void bladOdczytu(){
    printf("blad odczytu pliku %s",filename);
    exit(10);
}

void generate(){
    int i;
    int randomData=open("/dev/urandom",O_RDONLY);
    if(randomData<0){printf("blad odczytu pliku /dev/urandom");return;}
    int fileGen=open(filename,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
    if(fileGen<0)bladOdczytu();
    char* myRandomData=malloc(wielkosc_rekordow);
    for(i=0;i<rekordy;i++){
        int a=read(randomData,myRandomData,wielkosc_rekordow);
	if(a==0)printf("blad czytania");
	//else printf("%s",myRandomData);
        int b=write(fileGen,myRandomData,wielkosc_rekordow);
	if(b<0)printf("blad zapisu %s\n",strerror(errno));
    }
    close(randomData);
    free(myRandomData);
}

void shuffle(){
    char* data=malloc(wielkosc_rekordow);
    char* last=malloc(wielkosc_rekordow);
    if(0==sys0lib1){
        int fileGen=open(filename,O_RDWR);
        if(fileGen<0)bladOdczytu();
        int i;
        for(i=rekordy;i>1;i--){
            int r=rand();
            r%=i;
            if(r==i-1)continue;
            lseek(fileGen,wielkosc_rekordow*r,SEEK_SET);
            read(fileGen,data,wielkosc_rekordow);
            lseek(fileGen,wielkosc_rekordow*(i-1),SEEK_SET);
            read(fileGen,last,wielkosc_rekordow);

            lseek(fileGen,wielkosc_rekordow*r,SEEK_SET);
            write(fileGen,last,wielkosc_rekordow);
            lseek(fileGen,wielkosc_rekordow*(i-1),SEEK_SET);
            write(fileGen,data,wielkosc_rekordow);
        }
        close(fileGen);
    }else{
        FILE* fileGen=fopen(filename,"r+");
        if(fileGen==NULL)bladOdczytu();
        int i;
        for(i=rekordy;i>1;i--){
            int r=rand();
            r%=i;
            if(r==i-1)continue;
            fseek(fileGen,wielkosc_rekordow*r,SEEK_SET);
            fread(data,wielkosc_rekordow,1,fileGen);
            fseek(fileGen,wielkosc_rekordow*(i-1),SEEK_SET);
            fread(last,wielkosc_rekordow,1,fileGen);

            fseek(fileGen,wielkosc_rekordow*r,SEEK_SET);
            fwrite(last,wielkosc_rekordow,1,fileGen);
            fseek(fileGen,wielkosc_rekordow*(i-1),SEEK_SET);
            fwrite(data,wielkosc_rekordow,1,fileGen);
        }
        fclose(fileGen);
    }
    free(data);
    free(last);
}

void sort(){
    char* data=malloc(wielkosc_rekordow);
    char* last=malloc(wielkosc_rekordow);
    int i,j;
    unsigned char a,b;
    if(0==sys0lib1){
        int fileGen=open(filename,O_RDWR);
        if(fileGen<0)bladOdczytu();
        for(i=0;i<rekordy-1;i++){
            for(j=i+1;j<rekordy;j++){
                lseek(fileGen,wielkosc_rekordow*i,SEEK_SET);
                read(fileGen,&a,1);
                lseek(fileGen,wielkosc_rekordow*j,SEEK_SET);
                read(fileGen,&b,1);
                if(a>b){
                    lseek(fileGen,wielkosc_rekordow*i,SEEK_SET);
                    read(fileGen,data,wielkosc_rekordow);
                    lseek(fileGen,wielkosc_rekordow*j,SEEK_SET);
                    read(fileGen,last,wielkosc_rekordow);

                    lseek(fileGen,wielkosc_rekordow*i,SEEK_SET);
                    write(fileGen,last,wielkosc_rekordow);
                    lseek(fileGen,wielkosc_rekordow*j,SEEK_SET);
                    write(fileGen,data,wielkosc_rekordow);
                }
            }
        }
    }else{
        FILE* fileGen=fopen(filename,"r+");
        if(fileGen==NULL)bladOdczytu();
        for(i=0;i<rekordy-1;i++){
            for(j=i+1;j<rekordy;j++){
                fseek(fileGen,wielkosc_rekordow*i,SEEK_SET);
                fread(&a,1,1,fileGen);
                fseek(fileGen,wielkosc_rekordow*j,SEEK_SET);
                fread(&b,1,1,fileGen);
                if(a>b){
                    fseek(fileGen,wielkosc_rekordow*i,SEEK_SET);
                    fread(data,wielkosc_rekordow,1,fileGen);
                    fseek(fileGen,wielkosc_rekordow*j,SEEK_SET);
                    fread(last,wielkosc_rekordow,1,fileGen);

                    fseek(fileGen,wielkosc_rekordow*i,SEEK_SET);
                    fwrite(last,wielkosc_rekordow,1,fileGen);
                    fseek(fileGen,wielkosc_rekordow*j,SEEK_SET);
                    fwrite(data,wielkosc_rekordow,1,fileGen);
                }
            }
        }
    }
    free(data);
    free(last);
}

int main(int argc,const char* argv[]){
    if(argc!=6){
        arg();
        return 1;
    }
    if(0==strcmp(argv[1],"sys")){
        sys0lib1=0;
    }else if(0==strcmp(argv[1],"lib")){
        sys0lib1=1;
    }else{
        arg();
        return 2;
    }
    if(0==strcmp(argv[2],"generate")){
        gen0shu1sort2=0;
    }else if(0==strcmp(argv[2],"shuffle")){
        gen0shu1sort2=1;
    }else if(0==strcmp(argv[2],"sort")){
        gen0shu1sort2=2;
    }else {
        arg();
        return 3;
    }
    filename=argv[3];
    rekordy=strtol(argv[4],NULL,10);
    if(rekordy<1){arg();return 4;}
    wielkosc_rekordow=strtol(argv[5],NULL,10);
    if(wielkosc_rekordow<1){arg();return 5;}
    srand(time(NULL)*getpid());
    if(0==gen0shu1sort2){
        generate();
    }else if(1==gen0shu1sort2){
        shuffle();
    }else{
        sort();
    }
}

