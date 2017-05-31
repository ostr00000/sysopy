#include "const.h"

char* suffix[SEM_NUM] = {
    "BARBER_NOT_SLEEP",
    "CLIENT_READY",
    "PAY_BARBER",
    "NEVER_SURRENDER",
    "FREE_CHAIRS",
    "DOOR_CAN_BE_USED"};

void print_info(enum who is_client){
    char* who = (is_client)? "Client": "Barber";
    char formated[20] = {0};
    struct timespec tp;
    IS_ERROR(clock_gettime(CLOCK_REALTIME, &tp), "read current time\n");
    strftime(&formated[0], 20, "%F %T", localtime(&tp.tv_sec));
    printf("[%s %ld %s %d]: ", formated, tp.tv_nsec, who, getpid());
}

#if defined(DEBUG) && DEBUG > 1
#define PRINT(name) do{ fprintf(stderr, "adr: %p, "#name": %d\n", (void*)&name, name); }while(0) 
void print_shm(barbershop* shop, pid_t* queue){
    fprintf(stderr, "size of barbershop: %ld\n", sizeof(*shop));
    fprintf(stderr,"barbershop adr: %p\n", (void*)shop);

    PRINT(shop->next_person_id);
    PRINT(shop->hair.length);
    PRINT(shop->last_taken_chair);
    PRINT(shop->size);

    fprintf(stderr,"queue adr: %p\n", (void*)queue);
    for(int i = 0; i < shop->size; i++){
        fprintf(stderr, "adr: %p, element %d = %d, \n",  (void*)&queue[i], i, queue[i]);
    }
}
#undef PRINT

#define PRINT(name) do{ sem_getvalue(sem[name], &var); fprintf(stderr, #name": %d\n", var); }while(0)
void print_sem(sem_t *sem[]){
    PRINT(BARBER_NOT_SLEEP);
    PRINT(CLIENT_READY);
    PRINT(PAY_BARBER);
    PRINT(NEVER_SURRENDER);
    PRINT(FREE_CHAIRS);
    PRINT(DOOR_CAN_BE_USED);
}
#undef PRINT
#endif //DEBUG

#ifdef OUTPUT
void create_log_file(pid_t pid){
    char filename[sizeof("./processXXXXX.txt")];
    sprintf(filename, "./process%d.txt", pid);
    file = fopen(filename, "w");
    dup2(fileno(file), STDERR_FILENO);
}
#endif //OUTPUT


