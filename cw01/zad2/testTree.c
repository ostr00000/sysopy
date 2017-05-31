#include "testTree.h"

Tree* tree;

void testsTree(){
    int n;

    /* stworzenie */
   testTime("utworzenie drzewa",utworzTree,0,0);

    /* wstawianie */
    testTimeReset();
    for(n=0;n<1000;n++){
        char *text=malloc(30);
        sprintf(text,"dodanie %d-tego elementu",n);
        testTime(text,dodajTree,n,1);
    }
    testTimePrint("wstawianie");

    /* sortowanie */
    testTime("sortowanie drzewa",sortTree,n,0);

    /* szukanie */
    testTimeReset();
    for(n=0;n<1000;n++){
        char *text=malloc(30);
        sprintf(text,"szukanie %d-tego elementu",n);
        testTime(text,findTree,n,1);
    }
    testTimePrint("szukanie");

    /* usuwanie */
    testTimeReset();
    for(n=0;n<1000;n++){
        char *text=malloc(30);
        sprintf(text,"usuwanie %d-tego elementu",n);
        testTime(text,deleteTree,n,1);
    }
    testTimePrint("usuwanie");
}

void utworzTree(int n){
    tree=treeCreate();
}

void dodajTree(int n){
    treeAdd(tree,tabDanych[n]);
}

void sortTree(int n){
    treeSort(tree,NAZWISKO);
}

void findTree(int n){
    treeFind(tree,names[n],surnames[n]);
}

void deleteTree(int n){
    treeRemove(tree,names[n],surnames[n],0);
}
