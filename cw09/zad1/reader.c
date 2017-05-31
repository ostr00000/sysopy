#include "zad1.h"

void find_books(int div){
    int finded = 0;
    for(int i=0; i<num_of_books; i++){
        if(books[i] % div == 0){
            finded++;
            if(print_info){
                printf("reader TID: %ld find number %d that is divisible by %d on position %d\n", gettid(), books[i], div, i);
            }
        }
    }
    printf("reader TID: %ld find %d numbers that are divisible by %d\n", gettid(), finded, div);
}

void read_books(){
    IS_ERROR(-1 == sem_wait(&change_info), "reader awaited to use sem change_info - start\n");    
    current_readers++;
    int turn = reader_turn;
    IS_ERROR(-1 == sem_post(&change_info), "change_info is free now - start\n");

    if(!turn){
        IS_ERROR(-1 == sem_wait(&wait_reader), "reader has permission to read\n");
    }
    
    find_books(1 + rand() % MAX_DIV);

    IS_ERROR(-1 == sem_wait(&change_info), "reader awaited to change_info - end\n");
    current_readers--; 
    if(!current_readers && !writer_is_unlocked){
        writer_is_unlocked = 1;
        IS_ERROR(-1 == sem_post(&wait_writer), "there is no readers, now writers also can use books\n");
    }
    IS_ERROR(-1 == sem_post(&change_info), "change_info is free now - end\n");
}


