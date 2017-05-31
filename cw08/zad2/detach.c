#include "main.h"

void end(){
    for(int i=0; i<num_of_threads; i++){
        errno = pthread_detach(threads[i]);
        IS_ERROR(0 != errno, "thread detached %d\n"comma i);
    }
}



void *search_records(void* arg){
    long tid = gettid();

    #if OPERATION == 4
    mask_signal();    
    #elif OPERATION == 3 || OPERATION == 5
    catch_signal();
    #endif
    
    #if OPERATION == 6 || defined DEBUG   
    int thread_id = *(int*)arg;
    #endif

    #if OPERATION == 6
    if(thread_id == 2){
        int zero = thread_id / (thread_id - 2);
        (void)zero;
    }
    #endif
    #ifdef DEBUG
    fprintf(stderr, "thread: %d has tid:%ld\n", thread_id, tid);
    #endif //DEBUG

    long size = num_of_records * sizeof(struct record);
    struct record* rec = malloc(size + 1);
    IS_ERROR(NULL == rec, STRCAT_THREAD("alocated memory\n", tid));
    *((char*)rec + size) = 0;
    int ret;

    while((ret = read(file_desc, rec, size))){
        IS_ERROR(-1 == ret, STRCAT_THREAD("read record num: %d\n", tid) comma rec->id);
        char* ptr = strstr(rec->text, search_word);
        if(ptr){
            printf("Thread %ld found searched word in record: %d\n", tid, rec->id);
        }
    }
    return NULL;
}


