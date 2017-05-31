#include "ksiazka.h"

List* listCreate(){
    List* ret=malloc(sizeof(List));
    ret->first=NULL;
    ret->last=NULL;
    return ret;
}

void listDelete(List* list,int usunac){
    ListNode* tmp;
    if(list==NULL)return;
    tmp=list->first;
    if(tmp!=NULL){
        while(tmp->next!=NULL){
            tmp=tmp->next;
            if(usunac)free(tmp->prev->val);
            free(tmp->prev);
        }
        if(usunac)free(tmp->val);
        free(tmp);
    }
    free(list);
}

void listAdd(List*list,Dane *dane){
    ListNode* tmp;
    if(list==NULL)return;
    tmp=malloc(sizeof(ListNode));
    tmp->next=NULL;
    tmp->val=dane;
    tmp->prev=list->last;
    if(list->first==NULL){
        list->first=list->last=tmp;
    }else{
        list->last->next=tmp;
        list->last=tmp;
    }
}

Dane* listFind(List* list,const char* imie,const char* nazw){
    ListNode* tmp;
    if(list==NULL)return NULL;
    for(tmp=list->first;tmp!=NULL;tmp=tmp->next){
        if(strcmp(tmp->val->imie,imie)==0 && strcmp(tmp->val->nazwisko,nazw)==0){
            return tmp->val;
        }
    }
    return NULL;
}

int listRemove(List *list,const char* imie,const char* nazw,int usunac){
    ListNode* tmp;
    if(list==NULL)return 0;
    for(tmp=list->first;tmp!=NULL;tmp=tmp->next){
        if(strcmp(tmp->val->imie,imie)==0 && strcmp(tmp->val->nazwisko,nazw)==0){
            if(tmp->prev!=NULL)tmp->prev->next=tmp->next;
            else list->first=tmp->next;
            if(tmp->next!=NULL)tmp->next->prev=tmp->prev;
            else list->last=tmp->prev;
            if(usunac)free(tmp->val);
            free(tmp);
            return 1;
        }
    }
    return 0;
}

void listPrint(List* list){
    ListNode* tmp;
    if(list==NULL)return;
    tmp=list->first;
    while(tmp!=NULL){
        danePrint(tmp->val);
        tmp=tmp->next;
    }
    printf("\n");
}

void listNodePrint(List* list){
    ListNode* tmp;
    if(list==NULL)return;
    tmp=list->first;
    while(tmp!=NULL){
        Dane* val=tmp->val;
        printf("imie: %s", val->imie);
        if(tmp->prev!=NULL)printf(", prev=%s",tmp->prev->val->imie);
        if(tmp->next!=NULL)printf(", next=%s",tmp->next->val->imie);
        tmp=tmp->next;
        printf("\n");
    }
    printf("\n");
}

void listSort(List* list,enum rodzaj_danych rod){
    List* sorted;
    if(list==NULL || list->first==NULL || list->first->next==NULL)return;
    sorted=listCreate();
    while(list->first!=NULL){
        ListNode*i,*finded=list->first;
        for(i=list->first->next;i!=NULL;i=i->next){
            int com=compare(i->val,finded->val,rod);
            if(com<0)finded=i;
        }
        if(finded->prev!=NULL)finded->prev->next=finded->next;
        else list->first=finded->next;
        if(finded->next!=NULL)finded->next->prev=finded->prev;
        else list->last=finded->prev;
        listAdd(sorted,finded->val);
        free(finded);
    }
    list->first=sorted->first;
    list->last=sorted->last;
    free(sorted);
}

int compare(Dane* lhs,Dane* rhs,enum rodzaj_danych rod){
    switch(rod){
    case IMIE:
        return strcmp(lhs->imie,rhs->imie);
    case NAZWISKO:
        return strcmp(lhs->nazwisko,rhs->nazwisko);
    case EMAIL:
        return strcmp(lhs->email,rhs->email);
    case TELEFON:
        return strcmp(lhs->tel,rhs->tel);
    default:
        return 0;
    }
}

void danePrint(Dane* dane){
    printf("imie: %s, nazwisko: %s, data urodzenia: , email: , tel: , adres: \n",dane->imie,dane->nazwisko);
}

Tree* treeCreate(){
    Tree* ret=malloc(sizeof(Tree));
    ret->root=NULL;
    ret->rod=IMIE;
    return ret;
}

void treeDelete(Tree* tree,int usunac){
    treeNodeDel(tree->root,usunac);
}

void treeNodeDel(TreeNode* node,int usunac){
    if(node!=NULL){
        treeNodeDel(node->right,usunac);
        treeNodeDel(node->left,usunac);
        if(usunac)free(node->val);
        free(node);
    }
}

void treeAdd(Tree* tree,Dane* dane){
    TreeNode* created,*tmp;
    if(tree==NULL)return;
    created=malloc(sizeof(TreeNode));
    created->val=dane;
    created->left=NULL;
    created->right=NULL;
    tmp=tree->root;
    if(tree->root==NULL){
        tree->root=created;
    }else{
        while(1){
            int com=compare(tmp->val,dane,tree->rod);
            if(com>0){
                if(tmp->left==NULL){
                    tmp->left=created;
                    break;
                }else{
                    tmp=tmp->left;
                }
            }else{
                if(tmp->right==NULL){
                    tmp->right=created;
                    break;
                }else{
                    tmp=tmp->right;
                }
            }
        }
    }
    created->parent=tmp;
}

TreeNode* treeFindNode(Tree* tree,const char* imie,const char* nazw){
    TreeNode* tmp=tree->root;
    Dane* dane=malloc(sizeof(Dane));
    dane->imie=imie;
    dane->nazwisko=nazw;
    while(tmp!=NULL){
        int com=compare(tmp->val,dane,tree->rod);
        if(com==0)break;
        if(com>0){
            tmp=tmp->left;
        }else{
            tmp=tmp->right;
        }
    }
    free(dane);
    if(tmp==NULL)return NULL;
    else return tmp;
}

Dane* treeFind(Tree* tree,const char* imie,const char* nazw){
    if(tree==NULL || tree->root==NULL)return NULL;
    if(tree->rod!=IMIE && tree->rod!=NAZWISKO)return NULL;
    return treeFindNode(tree,imie,nazw)->val;

}

TreeNode* succ(TreeNode* node){
    TreeNode* tmp;
    if(node!=NULL){
        if(node->right!=NULL){
            return treeMin(node->right);
        }else{
            tmp=node->parent;
            while(tmp!=NULL && node==tmp->right){
                node=tmp;
                tmp=tmp->parent;
            }
            return tmp;
        }
    }
    return node;
}

TreeNode* treeMin(TreeNode* node){
    if(node!=NULL){
        while(node->left!=NULL){
            node=node->left;
        }
    }
    return node;
}

int treeRemove(Tree* tree,const char* imie,const char*nazw,int usunac){
    if(tree==NULL || tree->root==NULL)return 0;
    if(tree->rod!=IMIE && tree->rod!=NAZWISKO)return 0;
    else{
        TreeNode* node=treeFindNode(tree,imie,nazw);
        treeNodeRemove(tree,node,usunac);
    }
    return 1;
}

void treeNodeRemove(Tree* tree,TreeNode* node,int usunac){
    TreeNode* Y,*Z;
    if(usunac)free(node->val);
    if(node->left==NULL || node->right==NULL)Y=node;
    else Y=succ(node);
    if(Y->left!=NULL)Z=Y->left;
    else Z=Y->right;
    if(Z!=NULL)Z->parent=Y->parent;

    if(Y->parent==NULL)tree->root=Z;
    else if(Y==Y->parent->left)Y->parent->left=Z;
    else Y->parent->right=Z;
    if(Y!=node)node->val=Y->val;

    if(usunac)free(Y);
}

void treeNodePrint(Tree* tree){
    TreeNode* node=treeMin(tree->root);
    while(node!=NULL){
        printf("imie %s",node->val->imie);
        if(node->parent!=NULL)printf(" parent=%s",node->parent->val->imie);
        if(node->right!=NULL)printf(" right=%s",node->right->val->imie);
        if(node->left!=NULL)printf(" left=%s",node->left->val->imie);
        printf("\n");
        node=succ(node);
    }
    printf("\n");
}

void treePrint(Tree* tree){
    TreeNode* node=treeMin(tree->root);
    while(node!=NULL){
        danePrint(node->val);
        node=succ(node);
    }
    printf("\n");
}

void treeSort(Tree* tree, enum rodzaj_danych typ){
    if(typ==tree->rod)return;
    else{
        Tree* created=treeCreate();
        created->rod=typ;
        while(tree->root!=NULL){
            TreeNode* min=treeMin(tree->root);
            treeNodeRemove(tree,min,0);
            treeAdd(created,min->val);
            free(min);
        }
        tree->root=created->root;
        tree->rod=typ;
        free(created);
    }
}

