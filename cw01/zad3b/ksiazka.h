#ifndef KSIAZKA
#define KSIAZKA

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum rodzaj_danych {IMIE,NAZWISKO,EMAIL,TELEFON};

typedef struct Dane{
    const char *imie,*nazwisko,*data_ur,*email,*tel,*adres;
}Dane;

typedef struct ListNode{
    struct ListNode* prev;
    struct ListNode* next;
    Dane* val;
}ListNode;

typedef struct List{
    ListNode* first;
    ListNode* last;
}List;

/* tworzy pusta liste*/
List* listCreate(void);

/*usuwa cala liste wraz z elementami*/
void listDelete(List*,int);

/* dodaje dane do listy*/
void listAdd(List*,Dane*);

/* szuka osobe na liscie, szuka po imieniu i nazwisku,
zwraca wskaznik na dane znalezionej osoby, NULL gdy nie znaleziono*/
Dane* listFind(List*,const char*,const char*);

/*usuwa z listy 1. dane osoby, szuka po imieniu i nazwisku,
zwraca 1,gdy usunieto, 0 gdy nie znaleziono takiej osoby,
w zaleznosci od parametru kasuje elementy*/
int listRemove(List*,const char*,const char*,int);

/* funkcja pomocnicza, wypisuje zaleznosci na liscie*/
void listNodePrint(List*);

/*funkcja pomocnicza wypisuje wartosci daych*/
void listPrint(List*);

/* tworzy nowa posortowana liste*/
void listSort(List*,enum rodzaj_danych);

/*funkcja do porownywania przy sortowaniu */
int compare(Dane*,Dane*,enum rodzaj_danych);

void danePrint(Dane* dane);

typedef struct TreeNode{
    Dane* val;
    struct TreeNode* parent;
    struct TreeNode* left;
    struct TreeNode* right;
}TreeNode;

typedef struct Tree{
    TreeNode* root;
    enum rodzaj_danych rod;
}Tree;

/* tworzy puste drzewo, domyslnie posortowane po imieniu*/
Tree* treeCreate();

/*usuwa cale drzewo,w zaleznosci od parametru kasuje przechowywane elementy*/
void treeDelete(Tree*,int);

/*pomocnicza, usuwa wezel drzewa, w zaleznosci od parametru kasuje elementy*/
void treeNodeDel(TreeNode*,int);

/*dodaje wartosc w posortowany sposob*/
void treeAdd(Tree*,Dane*);

/*pomocnicza, znajduje wezel z imieniem i nazwiskiem*/
TreeNode* treeFindNode(Tree*,const char*,const char*);

/* znajduje dane, szuka po imieniu i nazwisku,
wymaga wczesniejszego posortowania po imieniu albo nazwisku*/
Dane* treeFind(Tree*,const char*,const char*);

/*pomocnicza, wyszukauje najmniejszy element, po lewej na dole*/
TreeNode* treeMin(TreeNode*);

/*pomocnicza, wyszukuje nastepnika*/
TreeNode* succ(TreeNode*);

/*usuwa element z drzewa, w zaleznosci od parametru kasuje go, wyszukuje go po imieniu i nazwisku
wymaga wczesniejszego posortowania po imieniu albo nazwisku*/
int treeRemove(Tree*,const char*,const char*,int);

/* usuwa wezel z drzewa, w zaleznosci od parametru kasuje go*/
void treeNodeRemove(Tree*,TreeNode*,int);


void treeSort(Tree*,enum rodzaj_danych);

/* pomocnicz wypisuje zaleznosci w drzewie*/
void treeNodePrint(Tree* tree);

void treePrint(Tree* tree);

#endif /* KSIAZKA */
