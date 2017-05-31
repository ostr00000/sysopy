#include "testTime.h"

clock_t time_min,time_max;
struct tms tms_min,tms_max;

void testTimeReset(){
    time_min=time_max=clock();
}

void testTime(char str[],void (*fun)(int),int n,int save){
    clock_t time_end;
    clock_t time_start=clock();
    struct tms tms_start,tms_end;
    times(&tms_start);
    fun(n);
    time_end=clock();
    times(&tms_end);
    if(save){
        if(time_end-time_start<time_min){
            time_min=time_end-time_start;
            tms_min.tms_utime=tms_end.tms_utime-tms_start.tms_utime;
            tms_min.tms_stime=tms_end.tms_stime-tms_start.tms_stime;
        }
        if(time_end-time_start>time_max){
            time_max=time_end-time_start;
            tms_max.tms_utime=tms_end.tms_utime-tms_start.tms_utime;
            tms_max.tms_stime=tms_end.tms_stime-tms_start.tms_stime;
        }
    }else{
        printf("%s   czas rzeczywisty = %.5f     czas urzytkownika = %.5f       czas systemowy = %.5f\n",str,
           ((double)(time_end-time_start)/(double)CLOCKS_PER_SEC),
           ((double)(tms_end.tms_utime-tms_start.tms_utime)/(double)CLOCKS_PER_SEC),
           ((double)(tms_end.tms_stime-tms_start.tms_stime)/(double)CLOCKS_PER_SEC));
    }
}

void testTimePrint(char text[]){
    printf("%s w przypadku optymistycznym trwalo:   czas rzeczywisty = %.5f     czas urzytkownika = %.5f    czas systemowy = %.5f\n",
           text,
           ((double)time_min)/(double)CLOCKS_PER_SEC,
           ((double)tms_min.tms_utime)/(double)CLOCKS_PER_SEC,
           ((double)tms_min.tms_stime)/(double)CLOCKS_PER_SEC
           );
    printf("%s w przypadku pesymistycznym trwalo:   czas rzeczywisty = %.5f     czas urzytkownika = %.5f    czas systemowy = %.5f\n",
           text,
           (double)time_max/(double)CLOCKS_PER_SEC,
           (double)tms_max.tms_utime/(double)CLOCKS_PER_SEC,
           (double)tms_max.tms_stime/(double)CLOCKS_PER_SEC
           );
}
