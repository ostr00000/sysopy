#include "zad1.h"

void overwrite_books(){
    int num_of_books_to_edit = 1 + rand() % (num_of_books / 2);
    for(int i=0; i<num_of_books_to_edit; i++){
        int book_to_edit = rand() % num_of_books;
        int new_val = 1 + rand() % MAX_NUMBER;
        books[book_to_edit] = new_val;
        if(print_info){
            printf("writer TID: %ld write number %d on position %d\n", gettid(), new_val, book_to_edit);
        }
    }
    printf("writer TID: %ld write %d books\n", gettid(), num_of_books_to_edit);
}

void write_books(){
    IS_ERROR(-1 == sem_wait(&wait_writer), "one writer want to write\n");

    IS_ERROR(-1 == sem_post(&change_info), "change_info is free now - start\n");
    reader_turn = 0;
    IS_ERROR(-1 == sem_post(&change_info), "change_info is free now - start\n");

    overwrite_books();

    IS_ERROR(-1 == sem_wait(&change_info), "writer awaited to use sem change_info - end\n");
    reader_turn = 1;    
    if(current_readers){     
        writer_is_unlocked = 0;  
        for(int i=current_readers; i>0; i--){
            IS_ERROR(-1 == sem_post(&wait_reader), "allow reader %d to use books\n"comma i);
        }
    }else{
        writer_is_unlocked = 1;
        IS_ERROR(-1 == sem_post(&wait_writer), "allow writer to use books\n");
    }
    IS_ERROR(-1 == sem_post(&change_info), "change_info is free now - end\n");
}

