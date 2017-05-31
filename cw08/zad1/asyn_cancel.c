#include "main.h"

void end(){
    for(int i=0; i<num_of_threads; i++){
        errno = pthread_join(threads[i], NULL);
        IS_ERROR(0 != errno, "thread joined %d\n"comma i);
    }
}

void cancel(int from, int to){
    for(int i=from; i<to; i++){
        errno = pthread_cancel(threads[i]);
        IS_ERROR(0 != errno, STRCAT_THREAD("cancel thread %d\n", syscall(SYS_gettid)) comma i);
    }
}

void *search_records(void* arg){
    long tid = syscall(SYS_gettid);    

    errno = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    IS_ERROR(0 != errno, STRCAT_THREAD("set cancel state\n", tid));
    errno = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    IS_ERROR(0 != errno, STRCAT_THREAD("set cancel type\n", tid));
    int thread_id = *(int*)arg;

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
            if(0 == finded_flag){
                finded_flag = 1;
                cancel(0, thread_id);
                cancel(thread_id + 1, num_of_threads);
                break;
            }
        }
    }
    return NULL;
}


