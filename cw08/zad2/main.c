#include "main.h"

long gettid(){
    return syscall(SYS_gettid);
}

void arg_parse(int argc, char* argv[]){
    if(argc == 5){
        num_of_threads = strtol(argv[1], NULL, 10);
        filename = argv[2];
        num_of_records = strtol(argv[3], NULL, 10);
        search_word = argv[4];
        if(num_of_threads > 0 && num_of_records > 0){
            return;
        }
    }
    fprintf(stderr, "give 4 args: num_of_threads, filename, num_of_records, search_word\n");
    exit(1);
}
#if OPERATION == 2 || OPERATION == 4
void mask_signal(){
    struct sigaction sig;
    IS_ERROR(-1 == sigfillset(&sig.sa_mask), "create full set mask\n");
    IS_ERROR(-1 == pthread_sigmask(SIG_SETMASK, &sig.sa_mask, NULL), "set full set mask\n");
}
#endif

#if  OPERATION == 3 || OPERATION == 5
void print_info(int sig){
    (void)sig;
    printf("PID: %d TID: %ld recieved signal \"%s\"\n", getpid(), gettid(), strsignal(TEST));
}

void catch_signal(){
    struct sigaction sig;
    sig.sa_handler = print_info;
    IS_ERROR(-1 == sigfillset(&sig.sa_mask), "create full set mask\n");
    sig.sa_flags = 0;
    IS_ERROR(-1 == sigaction(TEST, &sig, NULL), "signal \"%s\" masked\n"comma strsignal(TEST));
}
#endif //OPERATION == 3

#if OPERATION < 4
void send_to_proces(){
    pid_t child, parent = getpid();
    child = fork();
    if(0 == child){
        IS_ERROR(0 != kill(parent, TEST), "signal \"%s\" send\n"comma strsignal(TEST));
        exit(0);
    }
    IS_ERROR(-1 == child, "fork succes\n");
}
#elif OPERATION == 4 || OPERATION == 5
void send_to_thread(){
    IS_ERROR(0 != pthread_kill(threads[0], TEST), "signal \"%s\" send\n"comma strsignal(TEST));
}
#endif


int main(int argc, char* argv[]){    
    arg_parse(argc, argv);
    #if OPERATION == 3
    catch_signal();
    #elif OPERATION == 2
    mask_signal();
    #endif 
    finded_flag = 0;
    file_desc = open(filename, O_RDONLY);
    IS_ERROR(-1 == file_desc, "file opened\n");

    threads = malloc(sizeof(pthread_t*) * num_of_threads);
    IS_ERROR(NULL == threads, "allocated space for threads pointers\n");
    for(int *id,i=0; i<num_of_threads; i++){
        id = malloc(sizeof(int));
        IS_ERROR_NO_PRINT(NULL == id);
        *id = i;
        errno = pthread_create(&threads[i], NULL, search_records, id);
        IS_ERROR(0 != errno, "new thread created %d\n"comma i);
        if(i == num_of_threads/2){
            #if OPERATION == 4 || OPERATION == 5
            send_to_proces();
            #elif OPERATION < 4
            send_to_thread();
            #endif
        }
    }
    #if OPERATION < 4
    IS_ERROR(-1 == wait(NULL), "child returned\n");
    #endif //OPERATION < 4
    end();
    return 0;
}

