#include "const.h"

#ifdef OUTPUT
FILE* file = NULL;
#endif

int chairs;
int sem,shm;
barbershop* shop;
pid_t* queue;
int next_person = 0; 

void asleep(){
    print_info(BARBER);
    printf("Falling  asleep\n");

    IS_ERROR(semctl(sem, BARBER_IS_SLEEPING, SETVAL, 1), "go to sleep\n");
    
    struct sembuf action;
    action.sem_num = BARBER_IS_SLEEPING;
    action.sem_op = 0; /* barber wait while sb wake up him */
    action.sem_flg = 0;
    IS_ERROR(semop(sem, &action, 1), "end of sleep\n");
}

int get_queue_size(){
    int ret;
    IS_ERROR_VAL((ret = semctl(sem, FREE_CHAIRS, GETVAL)), "queue size is: ", ret);
    return ret;
}

pid_t check_waiting_room(){
    struct sembuf action;
    action.sem_num = DOOR_CAN_BE_USED;
    action.sem_op = -1;
    action.sem_flg = 0;
    IS_ERROR(semop(sem, &action, 1), "open door\n");

    action.sem_op = 1;    
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
            action.sem_num = FREE_CHAIRS;
            IS_ERROR(semop(sem, &action, 1), "clearing queue\n");
            action.sem_num = DOOR_CAN_BE_USED;   
        }
    }
    
    IS_ERROR(semop(sem, &action, 1), "close door\n");
    return person;
}

void end_program(int sig){
    fprintf(stderr, "\nBarber PID = %d recieved %s - closing\n", getpid(), strsignal(sig));
    exit(EXIT_SUCCESS);
}

void end(){
    IS_ERROR(semctl(sem, 0, IPC_RMID), "semaphores removed\n");
    IS_ERROR(shmdt((void*)shop), "shared memory detached\n");
    IS_ERROR(shmctl(shm, IPC_RMID, NULL), "shared memory removed\n");
}

void init_semaphores(){
    IS_ERROR((sem = semget(BARBER_SEM_KEY, 8, 0666 | IPC_CREAT)), "semaphore created\n");
    IS_ERROR(semctl(sem, BARBER_IS_SLEEPING, SETVAL, 1), "initialize barber status\n");
    IS_ERROR(semctl(sem, FREE_CHAIRS, SETVAL, chairs), "initialize number of chairs\n");
    IS_ERROR(semctl(sem, DOOR_CAN_BE_USED, SETVAL, 1), "initialize closed door\n");
    IS_ERROR(semctl(sem, NEXT_CUST1, SETVAL, 1), "initialize next client1 semaphore\n");
    IS_ERROR(semctl(sem, NEXT_CUST2, SETVAL, 0), "initialize next client2 semaphore\n");
    IS_ERROR(semctl(sem, CLIENT_READY, SETVAL, 1), "initialize client status\n");
    IS_ERROR(semctl(sem, PAY_BARBER, SETVAL, 1), "initialize haircut status\n");
    IS_ERROR(semctl(sem, ANULUJ, SETVAL, 0), "initialize anuluj status\n");
}

void init_shared_memory(){
    shm = shmget(BARBER_SHM_KEY, sizeof(barbershop) + sizeof(pid_t) * chairs, 0666 | IPC_CREAT);
    IS_ERROR(shm, "shared memory created\n");
    void *adr = shmat(shm, NULL, 0);
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
    exit(EXIT_FAILURE);
}

void call_next_client(pid_t client){
    static int look_at = NEXT_CUST1;
    shop->next_person_id = client;

    struct sembuf action[2];
    action[0].sem_num = look_at;
    action[0].sem_op = -1;
    action[0].sem_flg = IPC_NOWAIT;
    look_at = (look_at == NEXT_CUST1)? NEXT_CUST2: NEXT_CUST1;
    action[1].sem_num = look_at;
    action[1].sem_op = 1;
    action[1].sem_flg = IPC_NOWAIT;
    
    IS_ERROR(semop(sem, &action[0], 2), "swaping semaphores\n");
}

void go_to_work(pid_t client){
    call_next_client(client);

    print_info(BARBER);
    printf("Cutting hairs of client %d\n", client);

    struct sembuf action[3];
    action[0].sem_num = CLIENT_READY;
    action[0].sem_op = 0;
    action[0].sem_flg = 0;
    action[1].sem_num = ANULUJ;
    action[1].sem_op = 0;
    action[1].sem_flg = IPC_NOWAIT;
    action[2].sem_num = ANULUJ;
    action[2].sem_op = -1;
    action[2].sem_flg = IPC_NOWAIT;

    int ret;
    while(1){
        ret = semop(sem, &action[0], 2);
        if(ret == -1 && errno == EAGAIN){ /* is somebody resign with waiting ? */
            if(-1 == queue[next_person]){ /* is client dead in queue befor start haircut ? */
                return; 
            }
            IS_ERROR(semop(sem, &action[2], 1), 
                    "somebody dead in queue, lucky I not waiting for their\n");
            continue;
        }
        IS_ERROR(ret, "client sat on haircut position\n");
        action[0].sem_op = 1;
        IS_ERROR(semop(sem, &action[0], 1), "set up client ready semaphore");
        break;
    }
    shop->next_person_id = -1;
    
    #if defined(SLEEP) && SLEEP >= 1
    sleep(SLEEP);
    #endif //SLEEP

    shop->hair.length--;

    change_semaphore(sem, PAY_BARBER, "cut is finished\n", -1);
    wait_for_semaphore(sem, CLIENT_READY, "client has been left\n");    

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

