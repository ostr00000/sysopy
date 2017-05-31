#include "const.h"

void print_info(enum who is_client){
    char* who = (is_client)? "Client": "Barber";
    char formated[20] = {0};
    struct timespec tp;
    IS_ERROR(clock_gettime(CLOCK_REALTIME, &tp), "read current time\n");
    strftime(&formated[0], 20, "%F %T", localtime(&tp.tv_sec));
    printf("[%s %ld %s %d]: ", formated, tp.tv_nsec, who, getpid());
}

void wait_for_semaphore(int sem, enum semaphore number, char* string){
    #if !defined(DEBUG) || DEBUG == 2
    (void)string;
    #endif

    struct sembuf action[2];
    action[0].sem_num = number;
    action[0].sem_op = 0;
    action[0].sem_flg = 0;
    action[1].sem_num = number;
    action[1].sem_op = 1;
    action[1].sem_flg = 0;
    IS_ERROR(semop(sem, &action[0], 2), string);
}

void change_semaphore(int sem, enum semaphore number, char* string, int val){
    #if !defined(DEBUG) || DEBUG == 2
    (void)string;
    #endif
    
    struct sembuf action;
    action.sem_num = number;
    action.sem_op = val;
    action.sem_flg = 0;
    IS_ERROR(semop(sem, &action, 1), string);
}

#if defined(DEBUG) && DEBUG > 1
#define PRINT(name) do{ fprintf(stderr, "adr: %p, "#name": %d\n", (void*)&name, name); }while(0) 
void print_shm(barbershop* shop, pid_t* queue){
    fprintf(stderr, "size of barbershop: %ld\n", sizeof(*shop));
    fprintf(stderr,"queue adr: %p\n", (void*)shop);

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

#define PRINT(name) do{ fprintf(stderr, #name": %d\n", semctl(sem, name, GETVAL)); }while(0)
void print_sem(int sem){
    PRINT(BARBER_IS_SLEEPING);
    PRINT(FREE_CHAIRS);
    PRINT(DOOR_CAN_BE_USED);
    PRINT(NEXT_CUST1);
    PRINT(NEXT_CUST2);
    PRINT(CLIENT_READY);
    PRINT(PAY_BARBER);
    PRINT(ANULUJ);
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


