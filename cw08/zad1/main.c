#include "main.h"

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

int main(int argc, char* argv[]){    
    arg_parse(argc, argv);
    finded_flag = 0;
    file_desc = open(filename, O_RDONLY);
    IS_ERROR(-1 == file_desc, "file opened\n");

    threads = malloc(sizeof(pthread_t*) * num_of_threads);
    IS_ERROR(NULL == threads, "allocated space for threads pointers\n");
    for(int *id,i=0; i<num_of_threads; i++){
        IS_ERROR_NO_PRINT(NULL == (id = malloc(sizeof(int))));
        *id = i;
        errno = pthread_create(&threads[i], NULL, search_records, id);
        IS_ERROR(0 != errno, "new thread created %d\n"comma i);
    }
    end();
    return 0;
}
