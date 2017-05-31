#define _XOPEN_SOURCE 500

#include <stdlib.h>
#include <stdio.h>
#include <ftw.h>
#include <time.h>
#include <linux/limits.h>




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
int rozmiar;

int fun(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf){
    if(tflag==FTW_F && sb->st_size<rozmiar){
        char per[11];
        char lastMod[20];
        permission(per,sb->st_mode);
        struct tm * timeinfo=localtime(&(sb->st_mtime));
        strftime(lastMod, 20, "%b %d %H:%M", timeinfo);
        printf("path: %s, size %i, permission %s, last modification %s \n\n\n",fpath,(int)sb->st_size,per,lastMod);
    }
    return 0;
}

int main(int argc,const char* argv[]){
    if(argc!=3){
        printf("podaj argumeny: sciezka rozmiar");
        return 1;
    }
    rozmiar=strtol(argv[2],NULL,10);
    char real_path[PATH_MAX+1];
    realpath(argv[1],real_path);
    nftw((const char*)&real_path,fun,10,0);
}
