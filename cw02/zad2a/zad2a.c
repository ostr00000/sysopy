#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <time.h>

int rozmiar;

void permission(char tab[11],__mode_t info){
    tab[0]=(S_ISDIR(info))?'d':'-';
    tab[1]=(info&S_IRUSR)?'r':'-';
    tab[2]=(info&S_IWUSR)?'w':'-';
    tab[3]=(info&S_IXUSR)?'x':'-';
    tab[4]=(info&S_IRGRP)?'r':'-';
    tab[5]=(info&S_IWGRP)?'w':'-';
    tab[6]=(info&S_IWGRP)?'x':'-';
    tab[7]=(info&S_IROTH)?'r':'-';
    tab[8]=(info&S_IWOTH)?'w':'-';
    tab[9]=(info&S_IXOTH)?'x':'-';
    tab[10]=0;
}

void przeszukajKatalog(char* dir_path){
    strcat(dir_path,"/");
    struct dirent* plik;
    DIR* katalog;
    if((katalog=opendir(dir_path))){
        char* real_path=malloc(PATH_MAX+1);
        char per[11];
        char lastMod[20];
        struct stat info;
        while((plik=readdir(katalog))){
            if(strcmp(".",plik->d_name)==0 || strcmp("..",plik->d_name)==0)continue;
            strcpy(real_path,dir_path);
            strcat(real_path,plik->d_name);

            lstat(real_path,&info);
            if(S_ISDIR(info.st_mode)){
                przeszukajKatalog(real_path);
                continue;
            }else if(!S_ISREG(info.st_mode))continue;
            if(info.st_size>rozmiar)continue;

            //realpath(plik->d_name,real_path);
            permission(per,info.st_mode);

            struct tm * timeinfo=localtime(&(info.st_mtime));
            strftime(lastMod, 20, "%b %d %H:%M", timeinfo);
            printf("path: %s, size: %d , permissions: %s , last modification: %s \n\n\n",real_path,(int)info.st_size,per,lastMod);

        }
        closedir(katalog);
        free(real_path);
    }else{
        printf("nie mozna otworzyc sciezki: %s",dir_path);
    }


}

int main(int argc,const char* argv[]){
    if(argc!=3){
        printf("podaj argumeny: sciezka rozmiar");
        return 1;
    }
    rozmiar=strtol(argv[2],NULL,10);
    char real_path[PATH_MAX+1];
    realpath(argv[1],real_path);
    przeszukajKatalog((char*)&real_path);
}

