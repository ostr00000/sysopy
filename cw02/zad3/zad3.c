#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

const char infoLoop[]={"\n x - koniec,\n 1 - ustawienie rygla do odczytu na wybrany znak pliku,\n 2 - ustawienie rygla do zapisu na wybrany znak pliku,\n 3 - wyświetlenie listy zaryglowanych znaków pliku (z informacją o numerze PID procesu będącego właścicielem rygla oraz jego typie - odczyt/zapis),\n 4 - zwolnienie wybranego rygla,\n 5 - odczyt (funkcją read) wybranego znaku pliku,\n 6 - zmiana (funkcją write) wybranego znaku pliku.\n"};
const char infoTime[]={" 0 - wersja nieblokująca - próba ustawienia rygla kończy się wypisaniem informacji o błędzie\n 1 - wersja blokująca - program czeka, aż ustawienie rygla będzie możliwe"};
const char infoZnak[]={" Podaj nr znaku: "};
const char infoZamiana[]={" Podaj znak do zamiany"};

int file;
int znak;
int wait;
__pid_t pid;

int pobierzDane(const char text[]){
    int zz;
    printf("%s\n",text);
    fseek(stdin,0,SEEK_END);
    scanf("%d",&zz);
    return zz;
}

int wczytaj_dane(int w){
    znak=pobierzDane(infoZnak);
    wait=(w)?pobierzDane(infoTime):0;
    if(wait!=0 && wait!=1){
        printf("podales zly znak");
        return 0;
    }
    return 1;
}

struct flock flock_constr(int type,off_t of){
    struct flock zamek;
    zamek.l_len=1;//liczba bajtow do zablokowania
    zamek.l_whence=SEEK_SET;//miejsce skad liczymy offset
    zamek.l_type=type;//typ
    zamek.l_start=of;
    zamek.l_pid=pid;
    return zamek;
}

int main(int argc,const char* argv[]){
    if(argc!=2){
        printf("podaj argument: nazwaPliku\n");
        return 1;
    }
    file=open(argv[1],O_RDWR);
    if(file<0){
        printf("nie mozna otworzyc pliku %s\n",argv[1]);
        return 2;
    }
    struct flock zamek;
    pid=getpid();
    char comand;
    printf("%s",infoLoop);
    while(1){
        fseek(stdin,0,SEEK_END);
        scanf("%c",&comand);
        switch(comand){
        case 'x':
            return 0;
        case '1':{
            if(!wczytaj_dane(1))continue;
            zamek=flock_constr(F_RDLCK,znak);
            if(-1==fcntl(file,(wait)?F_SETLKW:F_SETLK,&zamek))perror(NULL);
            else printf("zablokowano odczyt %i znaku\n",znak);
            break;
        }
        case '2':{
            if(!wczytaj_dane(1))continue;
            zamek=flock_constr(F_WRLCK,znak);
            if(-1==fcntl(file,(wait)?F_SETLKW:F_SETLK,&zamek))perror(NULL);
            else printf("zablokowano zapis %i znaku\n",znak);
            break;
        }
        case '3':{
            printf("lista zalozonych rygli: \n");
            for(int i=0,size=lseek(file,0,SEEK_END);i<size;i++){
                zamek=flock_constr(F_RDLCK,i);
                fcntl(file,F_GETLK,&zamek);
                if(F_UNLCK!=zamek.l_type){
                    printf("znak %i ma zablokowany odczyt przez %i\n",(int)zamek.l_start,zamek.l_pid);
                }
                zamek=flock_constr(F_WRLCK,i);
                fcntl(file,F_GETLK,&zamek);
                if(F_UNLCK!=zamek.l_type){
                    printf("znak %i ma zablokowany zapis przez %i\n",(int)zamek.l_start,zamek.l_pid);
                }
            }
            break;
        }
        case '4':{
            if(!wczytaj_dane(0))continue;
            zamek=flock_constr(F_UNLCK,znak);
            if(-1==fcntl(file,F_SETLK,&zamek))perror(NULL);
            else printf("oblokowano znak %i pliku\n",znak);
            break;
        }
        case '5':{
            if(!wczytaj_dane(0))continue;
            lseek(file,znak,SEEK_SET);
            char readed;
            read(file,&readed,1);
            printf("%i znak w pliku to \'%c\'\n",znak,readed);
            break;
        }
        case '6':{
            if(!wczytaj_dane(0))continue;
            char do_zamiany;
            printf("%s\n",infoZamiana);
            fseek(stdin,0,SEEK_END);
            scanf(" %c",&do_zamiany);
            lseek(file,znak,SEEK_SET);
            write(file,&do_zamiany,1);
            printf("znak %c zapisany do pliku w na pozycji %i\n",do_zamiany,znak);
            break;
        }
        default:
            printf("%s",infoLoop);
        }
    }
}
