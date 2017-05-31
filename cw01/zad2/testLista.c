#include "testLista.h"

List* list;

void testsLista(){
    int n;

    /* stworzenie */
   testTime("utworzenie listy",utworzLista,0,0);

    /* wstawianie */
    testTimeReset();
    for(n=0;n<1000;n++){
        char *text=malloc(30);
        sprintf(text,"dodanie %d-tego elementu",n);
        testTime(text,dodajLista,n,1);
    }
    testTimePrint("wstawianie");

    /* sortowanie */
    testTime("sortowanie listy",sortLista,n,0);

    /* szukanie */
    testTimeReset();
    for(n=0;n<1000;n++){
        char *text=malloc(30);
        sprintf(text,"szukanie %d-tego elementu",n);
        testTime(text,findLista,n,1);
    }
    testTimePrint("szukanie");

    /* usuwanie */
    testTimeReset();
    for(n=0;n<1000;n++){
        char *text=malloc(30);
        sprintf(text,"usuwanie %d-tego elementu",n);
        testTime(text,deleteLista,n,1);
    }
    testTimePrint("usuwanie");
}

void utworzLista(int n){
    list=listCreate();
}

void dodajLista(int n){
    listAdd(list,tabDanych[n]);
}

void sortLista(int n){
    listSort(list,NAZWISKO);
}

void findLista(int n){
    listFind(list,names[n],surnames[n]);
}

void deleteLista(int n){
    listRemove(list,names[n],surnames[n],0);
}
