#include "zad1.h"

pthread_mutex_t writer = PTHREAD_MUTEX_INITIALIZER;

long gettid(){
    return syscall(SYS_gettid);
}

void parse(int argc, char* argv[]){
    if(argc >= 5){
        if(argc == 6 && 0 == strcmp("-i", argv[5])){
            print_info = 1;
        }
        num_of_writers = strtol(argv[1], NULL, 10);
        times_to_write = strtol(argv[2], NULL, 10);
        num_of_readers = strtol(argv[3], NULL, 10);
        times_to_read = strtol(argv[4], NULL, 10);
        if(num_of_writers >= 0
           && times_to_write > 0
           && num_of_readers >= 0
           && times_to_read > 0
           && num_of_writers + num_of_readers > 0){
            return;
        }
    }
    fprintf(stderr, "give 4 or 5 args: num_of_writers times_to_write num_of_readers times_to_read [-i]\n");
    exit(1);
}

void *start_writer(void* arg){
    (void)arg;
    for(int i = times_to_write; i>0; i--){
        write_books();
    }
    return NULL;
}

void *start_reader(void* arg){
    (void)arg;
    for(int i = times_to_read; i>0; i--){
        read_books();
    }
    return NULL;
}

void init(pthread_t** threads, int num_of_threads){
    srand(time(NULL));  
    for(int i=0; i<num_of_books; i++){
        books[i] = 1 + i;
    }
    writer_is_unlocked = 1;
    current_readers = 0;
    //sem_init(sem, {0-threads, non0-process}, start_val)
    IS_ERROR(-1 == sem_init(&wait_reader, 0, 0), "initialize wait_reader semaphore\n");
    IS_ERROR(-1 == sem_init(&wait_writer, 0, 1), "initialize wait_writer semaphore\n");
    IS_ERROR(-1 == sem_init(&change_info, 0, 1), "initialize change_info semaphore\n");
    *threads = malloc(sizeof(pthread_t*) * num_of_threads);
    IS_ERROR(NULL == *threads, "allocated space for threads pointers\n");
}

void run_threads(pthread_t* threads, int num_of_threads){
    int writers = num_of_writers;
    int readers = num_of_readers;
    for(int i=0; i<num_of_threads; i++){
        int create_writer = ((i%2==1 && writers>0) || !readers)? writers--: (readers--, 0);
        errno = pthread_create(&threads[i], NULL, (create_writer)? start_writer: start_reader, NULL);
        IS_ERROR(0 != errno, "new thread created %d\n"comma i);
    }
}

void end_threads(pthread_t* threads, int num_of_threads){
    for(int i=0; i<num_of_threads; i++){
        errno = pthread_join(threads[i], NULL);
        IS_ERROR(0 != errno, "thread joined %d\n"comma i);
    }
}

int main(int argc, char* argv[]){
    parse(argc, argv);

    int num_of_threads = num_of_writers + num_of_readers;
    pthread_t* threads;
    init(&threads, num_of_threads);
    run_threads(threads, num_of_threads);
    end_threads(threads, num_of_threads);
    return 0;
}

void print_info_sem(){
    for(int i=0; i< num_of_books; i++){
        printf("books[%d]=%d\n", i, books[i]);
    }
    printf("current_readers: %d, reader_turn: %d, writer_is_unlocked %d\n", 
            current_readers, reader_turn, writer_is_unlocked);
}


