#include "main.h"
#include <time.h>

int main(int argc, char* argv[]){
    int num;
    if(argc != 3 || 1>(num = strtol(argv[2], NULL, 10))){
        fprintf(stderr, "give 2 arg: filename, num_of_records\n");        
        return 1;
    }
    srand(time(NULL));
    int fd = open(argv[1], O_WRONLY | O_CREAT);
    IS_ERROR(-1 == fd, "file opened\n");
    struct record rec;
    for(int i=0; i<num; i++){
        rec.id = i;
        for(int j=0; j<TEXT_SIZE; j++){
            rec.text[j] = 'a' + rand() % ('z' - 'a' + 1);
        }
        IS_ERROR(-1 == write(fd, &rec, sizeof(rec)), "writing to file %d \n"comma i);
    }
    return 0;
}
