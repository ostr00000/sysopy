#include "const.h"

#ifdef OUTPUT
FILE* file = NULL;
#endif

int num_clients,num_haircuts;
pid_t* childs = NULL;
pid_t parent,child;
sem_t* sem[SEM_NUM];
int shm;
barbershop* shop = NULL;
int sit_in_queue = -1;
pid_t* queue = NULL;
haircut hair;

void end_program(int sig){
    fprintf(stderr, "\nProcess PID = %d recieved %s - closing\n", getpid(), strsignal(sig));
    exit(0);
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
    }
    //IS_ERROR_VAL(munmap((void*)queue, sizeof(barbershop) + sizeof(pid_t) * shop->size),
    //             "closed shared memory by process = ", my_pid);
}

void go_to_barber_chair(int sig){
    (void)sig;
    queue[sit_in_queue] = 0;
    sit_in_queue = -1;
    IS_ERROR(sem_post(sem[FREE_CHAIRS]), "leaving queue\n");

    shop->hair = hair;
    IS_ERROR(sem_post(sem[CLIENT_READY]), "client's hair ready to be cut\n");
    IS_ERROR(sem_wait(sem[PAY_BARBER]), "wait to cut\n");
    hair = shop->hair;
    IS_ERROR(sem_post(sem[CLIENT_READY]), "hair cuted, client is leaving shop\n");
    
    print_info(CLIENT);
    printf("Leaving barber with new haircut\n");
    print_sem(sem);
}

int try_book_chair(){
    sigset_t sig, old_sig;
    IS_ERROR(sigfillset(&sig), "create full filled signal set\n");
    IS_ERROR(sigprocmask(SIG_BLOCK, &sig, &old_sig), "set signal mask\n");
    IS_ERROR(sem_wait(sem[DOOR_CAN_BE_USED]), "door open\n");

    int ret = sem_trywait(sem[FREE_CHAIRS]);
    if(-1 == ret && errno == EAGAIN){ /* queue is full */
        IS_ERROR(sem_post(sem[DOOR_CAN_BE_USED]), "sitting on free chair in queue and close door\n");
        IS_ERROR(sigprocmask(SIG_SETMASK, &old_sig, NULL), "return old signal mask\n");
        return 0;
    }
    IS_ERROR(ret, "wait in waiting room\n");

    if(++shop->last_taken_chair == shop->size){
        shop->last_taken_chair = 0;
    }
    sit_in_queue = shop->last_taken_chair;
    queue[sit_in_queue] = child;
    IS_ERROR(sem_post(sem[BARBER_NOT_SLEEP]), "say to barber to not sleep\n");
    print_info(CLIENT);
    printf("Sitting in waiting room\n");
    
    IS_ERROR(sem_post(sem[DOOR_CAN_BE_USED]), "sitting on free chair in queue and close door\n");
    IS_ERROR(sigprocmask(SIG_SETMASK, &old_sig, NULL), "return old signal mask\n");
    print_sem(sem);
    if(-1 == sit_in_queue){
        return 1;
    }
    ret = sem_wait(sem[NEVER_SURRENDER]);
    if(-1 == ret && errno == EINTR){
        return 1;
    }
    IS_ERROR(ret, "this should be never seen");
    return -1;
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
    exit(EXIT_SUCCESS);
}

void init(){    
    parent = getpid();   

    #ifdef OUTPUT
    create_log_file(parent);
    #endif

    struct sigaction action;
    action.sa_handler = go_to_barber_chair;
    IS_ERROR(sigfillset(&action.sa_mask), "create full mask\n");
    action.sa_flags = 0;
    IS_ERROR(sigaction(SIGUSR1, &action, NULL), "signal USR1 has been handled\n");
    action.sa_handler = end;
    IS_ERROR(sigaction(SIGINT, &action, NULL), "signal INT has been handled\n");
    
    childs = (pid_t*) calloc(num_clients, sizeof(pid_t));
    IS_ERROR3(NULL, childs, "alocate space for childs\n");
    
    for(int i = 0; i < SEM_NUM; i++){
        IS_ERROR3_VAL(SEM_FAILED, (sem[i] = sem_open(suffix[i], O_RDWR | O_CREAT)), suffix[i], 0);
    }
    
    IS_ERROR((shm = shm_open(BARBER_SHM_KEY, O_RDWR, 0666)), "shared memory opened\n");
    void *adr = mmap(NULL, sizeof(barbershop), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    IS_ERROR3((void*)-1, adr, "adress of shared memory of shop attached\n");
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

            hair.length = num_haircuts;
            for(int j = 0; j < num_haircuts;){
                if(try_book_chair()){
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

