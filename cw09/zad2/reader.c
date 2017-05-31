#include "zad2.h"

void find_books(int div){
    int finded = 0;
    for(int i=0; i<num_of_books; i++){
        if(books[i] % div == 0){
            finded++;
            if(print_info){
                printf("Reader TID: %ld find number %d that is divisible by %d on position %d\n", gettid(), books[i], div, i);
            }
        }
    }
    printf("Reader TID: %ld find %d numbers that are divisible by %d\n", gettid(), finded, div);
}

void read_books(){
    IS_ERROR_E(pthread_mutex_lock(&queue_mutex), "queue mutex lock - start\n");
    int can_reader_go = queue_add_reader();
    if(!can_reader_go){
        #ifdef DEBUG
        fprintf(stderr, "TID: %ld queue mutex unlocked - going to wait\n", gettid());
        #endif 
        IS_ERROR_E(pthread_cond_wait(&reader_queue_chnage, &queue_mutex), "end wait, lock is set - readers turn\n");
    }
    IS_ERROR_E(pthread_mutex_unlock(&queue_mutex), "queue mutex unlock - start\n");

    find_books(1 + rand() % MAX_DIV);
    
    IS_ERROR_E(pthread_mutex_lock(&queue_mutex), "queue mutex lock - end\n");
    queue_rem_reader();
    IS_ERROR_E(pthread_mutex_unlock(&queue_mutex), "queue mutex unlock - end\n");
}


