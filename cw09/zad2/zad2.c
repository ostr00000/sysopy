#include "zad2.h"

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
    fprintf(stderr, "Give 4 or 5 args: num_of_writers times_to_write num_of_readers times_to_read [-i]\n");
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
    queue_init();
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&writer_queue_mutex, NULL);
    pthread_mutex_init(&reader_queue_mutex, NULL);
    pthread_cond_init(&writer_queue_chnage, NULL);
    pthread_cond_init(&reader_queue_chnage, NULL);
    *threads = malloc(sizeof(pthread_t*) * num_of_threads);
    IS_ERROR(NULL == *threads, "allocated space for threads pointers\n");
}

void run_threads(pthread_t* threads, int num_of_threads){
    int writers = num_of_writers;
    int readers = num_of_readers;
    for(int create_writer,i=0; i<num_of_threads; i++){
        if((i%2==1 && writers>0) || !readers){
            create_writer = 1;
            writers--;
        }else{
            create_writer = 0;            
            readers--;
        }
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

void print_info_data(){
    for(int i=0; i< num_of_books; i++){
        printf("books[%d]=%d\n", i, books[i]);
    }
}

void print_info_queue(){
    for(int i=0; i<num_of_writers+1; i++){
        printf("queue[%d]=%d\n", i, queue.start[i]);
    }
    fprintf(stderr, "readers[0]=%d, readers[1]=%d current_readers=%d\n", queue.readers[0], queue.readers[1], queue.current_reader);
    fprintf(stderr, "current_pos=%d, last_pos=%d\n", queue.current_pos, queue.last_pos);
}

void queue_init(){
    queue.start = calloc(num_of_writers + 1, sizeof(int));
    IS_ERROR(NULL == queue.start, "space for queue alocated\n");
    queue.current_pos = queue.last_pos = 0;
    queue.readers[0] = queue.readers[1] = queue.current_reader = 0;
}

int queue_add_writer(){
    int ret = queue.last_pos;    
    queue.start[ret] = gettid();
    /* if index is too hight - reset */
    if(++queue.last_pos >= num_of_writers + 1){ 
        queue.last_pos = 0;
    }
    return ret;
}

int queue_add_reader(){
    /* if no writer wait, allow reader go */
    if(queue.current_pos == queue.last_pos){ 
        queue.readers[queue.current_reader]++;
        return 1;
    }
    /* if writers in queue, allow when writer end */
    queue.readers[(queue.current_reader)? 0: 1]++;
    return 0;
}

int queue_get_next_id(){
    return queue.current_pos;
}

int queue_is_readers_turn(){
    return queue.readers[queue.current_reader];
}

void queue_rem_writer(){
    queue.start[queue.current_pos] = 0;
    queue.current_pos++;
    /* if index is too hight - reset */
    if(queue.current_pos >= num_of_writers + 1){
        queue.current_pos = 0;
    }
    queue.current_reader = (queue.current_reader)? 0: 1;
    /* if there are readers allow them */
    if(queue.readers[queue.current_reader]){
        IS_ERROR_E(pthread_cond_broadcast(&reader_queue_chnage), "reader turn now\n");
    }else{
        IS_ERROR_E(pthread_cond_broadcast(&writer_queue_chnage), "writer turn again\n");
    }
}

void queue_rem_reader(){
    queue.readers[queue.current_reader]--;
    /* if all readers end read*/
    if(queue.readers[queue.current_reader] == 0){
        /* if writer wait in queue */
        if(queue.current_pos != queue.last_pos){
            IS_ERROR_E(pthread_cond_broadcast(&writer_queue_chnage), "writer turn now\n");
        }
    }
}


