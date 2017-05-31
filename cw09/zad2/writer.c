#include "zad2.h"

void overwrite_books(){
    int num_of_books_to_edit = 1 + rand() % (num_of_books / 2);
    for(int i=0; i<num_of_books_to_edit; i++){
        int book_to_edit = rand() % num_of_books;
        int new_val = 1 + rand() % MAX_NUMBER;
        books[book_to_edit] = new_val;
        if(print_info){
            printf("Writer TID: %ld write number %d on position %d\n", gettid(), new_val, book_to_edit);
        }
    }
    printf("Writer TID: %ld write %d books\n", gettid(), num_of_books_to_edit);
}

void write_books(){
    IS_ERROR_E(pthread_mutex_lock(&queue_mutex), "queue mutex lock - writer - start\n");
    int number_in_queue = queue_add_writer();
    while(number_in_queue != queue_get_next_id() || queue_is_readers_turn()){
        #ifdef DEBUG
        fprintf(stderr, "TID: %ld queue mutex unlocked - going to wait - writer\n", gettid());
        #endif     
        IS_ERROR_E(pthread_cond_wait(&writer_queue_chnage, &queue_mutex), "check queue, lock is set - writer\n");
    }
    IS_ERROR_E(pthread_mutex_unlock(&queue_mutex), "queue mutex unlock - writer - start\n");
    
    overwrite_books();
    
    IS_ERROR_E(pthread_mutex_lock(&queue_mutex), "queue mutex lock - writer - end\n");
    queue_rem_writer();
    IS_ERROR_E(pthread_mutex_unlock(&queue_mutex), "queue mutex unlock - writer - end\n");
}

