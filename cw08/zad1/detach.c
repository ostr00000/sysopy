#include "main.h"

void end(){
    for(int i=0; i<num_of_threads; i++){
        errno = pthread_detach(threads[i]);
        IS_ERROR(0 != errno, "thread detached %d\n"comma i);
    }
}



void *search_records(void* arg){
    long tid = syscall(SYS_gettid);    
    
    #ifdef DEBUG
    int thread_id = *(int*)arg;
    fprintf(stderr, "thread: %d has tid:%ld\n", thread_id, tid);
    #else
    (void)arg;
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


