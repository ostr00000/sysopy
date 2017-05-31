#include "const.h"

#ifdef OUTPUT
FILE* file = NULL;
#endif

int num_clients,num_haircuts;
pid_t* childs = NULL;
pid_t parent,child;
int sem,shm;
barbershop* shop = NULL;
int sit_in_queue = -1; /* -1 mean not in queue */
pid_t* queue = NULL;

void end_program(int sig){
    fprintf(stderr, "\nProcess PID = %d recieved %s - closing\n", getpid(), strsignal(sig));
    exit(EXIT_SUCCESS);
}

void end(){
    pid_t my_pid = getpid();
    if(my_pid == parent){
        if(childs){
            for(int i = 0; i < num_clients; i++){
                if(childs[i]){        
                    kill(childs[i], SIGINT);
                }
            }
        }
    }else if(-1 != sit_in_queue){
        queue[sit_in_queue] = -1;
        change_semaphore(sem, ANULUJ, "set anuluj semaphore\n", 1);
    }
}

void try_wake_up_barber(){
    struct sembuf action;
    action.sem_num = BARBER_IS_SLEEPING;
    action.sem_op = -1;
    action.sem_flg = IPC_NOWAIT;

    int ret = semop(sem, &action, 1);
    if(-1 == ret && errno == EAGAIN){
        return;
    }
    IS_ERROR(ret, "trying wakeup barber\n");
    print_info(CLIENT);
    printf("Waking up barber\n");    
}

void wait_in_queue(){
    print_info(CLIENT);
    printf("Sitting in waiting room\n");

    struct sembuf action;
    action.sem_op = 0;
    action.sem_flg = 0;
    while(shop->next_person_id != child){
        action.sem_num = NEXT_CUST1;
        IS_ERROR(semop(sem, &action, 1), "waiting in queue\n");
        if(shop->next_person_id == child){
            return;
        }
        action.sem_num = NEXT_CUST2;
        IS_ERROR(semop(sem, &action, 1), "waiting in queue\n");
    }
}

int try_book_chair(haircut* hair){
    struct sembuf action[2];
    action[0].sem_num = FREE_CHAIRS;
    action[0].sem_op = -1;
    action[0].sem_flg = IPC_NOWAIT;
    action[1].sem_num = DOOR_CAN_BE_USED;
    action[1].sem_op = -1;
    action[1].sem_flg = 0;
    
    sigset_t sig, old_sig;
    IS_ERROR(sigfillset(&sig), "create full filled signal set\n");
    IS_ERROR(sigprocmask(SIG_BLOCK, &sig, &old_sig), "set signal mask\n");
    int ret = semop(sem, &action[0], 2);
    if(-1 == ret && errno == EAGAIN){ /* queue is full */
        IS_ERROR(sigprocmask(SIG_SETMASK, &old_sig, NULL), "return old signal mask\n");
        return 0;
    }
    IS_ERROR(ret, "open door to waiting room\n");

    if(++shop->last_taken_chair == shop->size){
        shop->last_taken_chair = 0;
    }
    sit_in_queue = shop->last_taken_chair;
    queue[sit_in_queue] = child;
    try_wake_up_barber();
    action[1].sem_op = 1;
    IS_ERROR(semop(sem, &action[1], 1), "sitting on free chair in queue and close door\n");
    IS_ERROR(sigprocmask(SIG_SETMASK, &old_sig, NULL), "return old signal mask\n");
    
    wait_in_queue();

    IS_ERROR(sigprocmask(SIG_BLOCK, &sig, NULL), "set signal mask\n");
    queue[sit_in_queue] = 0;
    sit_in_queue = -1;
    change_semaphore(sem, FREE_CHAIRS, "leaving queue\n", 1);

    shop->hair = *hair;
    change_semaphore(sem, CLIENT_READY, "client's hair ready to be cut\n", -1);
    wait_for_semaphore(sem, PAY_BARBER, "wait to cut and initialize pay semaphore\n");
    *hair = shop->hair;
    change_semaphore(sem, CLIENT_READY, "client is leaving shop\n", -1);

    print_info(CLIENT);
    printf("Leaving barber with new haircut\n");
    
    IS_ERROR(sigprocmask(SIG_SETMASK, &old_sig, NULL), "return old signal mask\n");
    return 1;
}

void parse_args(int argc, char* argv[]){
    if(argc == 3){
        num_clients = strtol(argv[1], NULL, 10);
        num_haircuts = strtol(argv[2], NULL, 10);
        if(num_clients >= 1 && num_haircuts >= 1){
            return;
        }
    }
    fprintf(stderr, "give 2 args: number_of_clients number_of_haircuts\n");
    exit(EXIT_FAILURE);
}

void init(){    
    parent = getpid();   

    #ifdef OUTPUT
    create_log_file(parent);
    #endif
    
    struct sigaction action;
    action.sa_handler = end_program;
    IS_ERROR(sigfillset(&action.sa_mask), "create full mask\n");
    action.sa_flags = 0;
    IS_ERROR(sigaction(SIGINT, &action, NULL), "signal INT has been handled\n");

    childs = (pid_t*) calloc(num_clients, sizeof(pid_t));
    IS_ERROR3(NULL, childs, "alocate space for childs\n");
    
    IS_ERROR((sem = semget(BARBER_SEM_KEY, 0, 0666)), "semaphore opened\n");

    IS_ERROR((shm = shmget(BARBER_SHM_KEY, 0, 0666)), "shared memory opened\n");
    void *adr = shmat(shm, NULL, 0);
    IS_ERROR3((void*)-1, adr, "adress of shared memory attached\n");
    shop = (barbershop*) adr;
    queue = (pid_t*) ((char*)adr + sizeof(barbershop));

    AT_EXIT(end);
}


void start_customers(){
    for(int i = 0; i < num_clients; i++){
        childs[i] = fork();
        if(childs[i] == 0){
            child = getpid();
            srand(child);

            #ifdef OUTPUT
            create_log_file(child);
            #endif

            haircut hair;
            hair.length = num_haircuts;
            for(int j = 0; j < num_haircuts;){
                if(try_book_chair(&hair)){
                    j++;                
                }else{
                    int sleep_time = 1 + rand() % MAX_TIME_SLEEP;
                    print_info(CLIENT);                    
                    printf("Queue is full, I go to home for %d sec\n", sleep_time);
                    sleep(sleep_time);
                }
            }
            #if defined(DEBUG) && DEBUG != 2
            fprintf(stderr, "length of my hair is now %d\n", hair.length);
            #endif
            exit(EXIT_SUCCESS);
        }
        IS_ERROR_VAL(childs[i], "create child number = ", i);
    }
}

void wait_for_end_customers(){
    int status;
    for(int i = 0; i < num_clients; i++){
        child = waitpid(childs[i], &status, 0);
        childs[i] = 0;
        IS_ERROR_VAL(child, "successfully retruned child = ", child);
        
        #if defined(DEBUG) && DEBUG != 2
        if(WIFEXITED(status)){        
            fprintf(stderr,"child PID = %d returned with satus = %d\n",
                    child, WEXITSTATUS(status));
        }else if(WIFSIGNALED(status)){
            fprintf(stderr,"child PID = %d terminated by signal %s\n",
                    child, strsignal(WTERMSIG(status)));
        }
        #endif
    }
}

int main(int argc, char* argv[]){    
    parse_args(argc, argv);    
    init();
    start_customers();
    wait_for_end_customers();
    return 0;
}

