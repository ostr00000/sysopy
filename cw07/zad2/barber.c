#include "const.h"

#ifdef OUTPUT
FILE* file = NULL;
#endif

int chairs;
int size;
sem_t* sem[SEM_NUM];
int shm;
barbershop* shop;
pid_t* queue;

void asleep(){
    print_info(BARBER);
    printf("Falling  asleep\n");
    IS_ERROR(sem_wait(sem[BARBER_NOT_SLEEP]), "waked up\n");
}

int get_queue_size(){
    int ret;
    IS_ERROR_VAL((sem_getvalue(sem[FREE_CHAIRS], &ret)), "queue size is: ", ret);
    return ret;
}

pid_t check_waiting_room(){
    static int next_person = 0;
    IS_ERROR(sem_wait(sem[DOOR_CAN_BE_USED]), "open door\n");
    pid_t person = 0;
    
    #ifdef DEBUG
    fprintf(stderr, "current person: %d\n", next_person);
    #endif //DEBUG
    
    if(next_person != shop->last_taken_chair || 0 == get_queue_size()){
        if(++next_person == shop->size){
            next_person = 0;
        }       
        person = queue[next_person];
        if(-1 == person){
            queue[next_person] = 0;
            IS_ERROR(sem_post(sem[FREE_CHAIRS]), "clearing queue\n");   
        }
    }
    IS_ERROR(sem_post(sem[DOOR_CAN_BE_USED]), "close door\n");
    return person;
}

void end_program(int sig){
    fprintf(stderr, "\nBarber PID = %d recieved %s - closing\n", getpid(), strsignal(sig));
    exit(EXIT_SUCCESS);
}

void end(){
    for(int i = 0; i < SEM_NUM; i++){
        IS_ERROR_VAL(sem_close(sem[i]), suffix[i], 0);
        IS_ERROR(sem_unlink(suffix[i]), "semaphore unlinked\n");
    }
    IS_ERROR(munmap((void*)shop, sizeof(barbershop) + sizeof(pid_t) * chairs),
             "shared memory detached\n");
    IS_ERROR(shm_unlink(BARBER_SHM_KEY), "shared memory removed\n");
}

void init_semaphores(){
    for(int i = 0, max = SEM_NUM - 2; i < max; i++){
        sem[i] = sem_open(suffix[i], O_RDWR | O_CREAT, 0666, 0); //digit 0 to end line
        IS_ERROR3_VAL(SEM_FAILED, sem[i], suffix[i], 1);
    }
    sem[FREE_CHAIRS] = sem_open(suffix[FREE_CHAIRS], O_RDWR | O_CREAT, 0666, chairs);
    IS_ERROR3_VAL(SEM_FAILED, sem[FREE_CHAIRS], suffix[FREE_CHAIRS], 1); //digit 1 to end line
    sem[DOOR_CAN_BE_USED] = sem_open(suffix[DOOR_CAN_BE_USED], O_RDWR | O_CREAT, 0666, 1);
    IS_ERROR3_VAL(SEM_FAILED, sem[DOOR_CAN_BE_USED], suffix[DOOR_CAN_BE_USED], 1); //digit 1 to end line
}

void init_shared_memory(){
    int size = sizeof(barbershop) + sizeof(pid_t) * chairs;
    IS_ERROR((shm = shm_open(BARBER_SHM_KEY, O_RDWR | O_CREAT | O_EXCL, 0666)), "shared memory created\n");
    IS_ERROR(ftruncate(shm, size), "size of shared memory defined\n");
     
    void *adr = mmap(NULL, size, PROT_WRITE | PROT_READ, MAP_SHARED, shm, 0);
    IS_ERROR3((void*)-1, adr, "adress of shared memory attached\n");

    shop = (barbershop*) adr;
    queue = (pid_t*) ((char*)adr + sizeof(barbershop));
    shop->size = chairs;
    shop->last_taken_chair = 0;
    shop->next_person_id = -1;
}

void parse_args(int argc,char* argv[]){
    if(argc == 2){
        chairs = strtol(argv[1], NULL, 10);
        if(chairs >= 2){
            return;
        }
    }
    fprintf(stderr, "give 1 argumnet: chairs_in_shop\n");
    exit(1);
}

void call_next_client(pid_t client){
    shop->next_person_id = client;
    IS_ERROR(kill(client, SIGUSR1), "send signal\n");
}

void go_to_work(pid_t client){
    call_next_client(client);

    print_info(BARBER);
    printf("Cutting hairs of client %d\n", client);
    
    IS_ERROR(sem_wait(sem[CLIENT_READY]), "client sat on haircut position\n");
    shop->next_person_id = -1;
    
    #if SLEEP >= 1
    sleep(SLEEP);
    #endif //SLEEP

    shop->hair.length--;

    IS_ERROR(sem_post(sem[PAY_BARBER]), "cut is finished\n");
    IS_ERROR(sem_wait(sem[CLIENT_READY]), "client has been left\n");    

    print_info(BARBER);
    printf("Hairs of client %d has been successfully cut\n", client);
}

int main(int argc, char* argv[]){
    parse_args(argc,argv);

    #ifdef OUTPUT
    create_log_file(getpid());
    #endif

    init_semaphores();
    init_shared_memory();
    AT_EXIT(end);
    IS_ERROR3(SIG_ERR, signal(SIGINT, end_program), "signal INIT has been handled\n");
    
    pid_t client;
    while(1){
        #if defined(DEBUG) && DEBUG > 1
        print_sem(sem);
        print_shm(shop, queue);
        #endif //DEBUG

        if((client = check_waiting_room())){
            if(-1 == client){ /* client dead in queue */           
                continue;
            }
            go_to_work(client);
        }else{
            asleep();
        }
    }
}

