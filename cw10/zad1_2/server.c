#include "const.h"

const int port_number;  //program arg
const char* pathname;   //program arg

pthread_t socket_thread, pinger_thread;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

client_info clients[MAX_CLIENTS];
char clients_in_array = 0;
int order_id = 0; //global order counter 

int epolld; //epoll descriptor
int lsd, nsd; //local socket descriptor, network socket descriptor
int exit_status = 0; //flag use while exiting

void parse_arg(int argc, char* argv[]){
    while(argc == 3){
        *(int*)&port_number = strtol(argv[1], NULL, 10);
        if(port_number < 1024 || port_number > 65535){
            break;// below 1024 need additional permission, max is 65535
        }
        if(strlen(argv[2]) + 1 > MAX_PATH_SIZE){
            fprintf(stderr, "socket_name is too long\n");
            exit(EXIT_FAILURE);
        }
        pathname = argv[2];
        return;
    }
    fprintf(stderr, "give 2 args: " PROTOCOL "_port_number{1024..65535} scoket_name\n");
    exit(EXIT_FAILURE);
}

void print_clients(){
    for(int i=0; i<clients_in_array; i++){
        #ifdef TCP
        fprintf(stderr, "client %d: id: %d, name: %s\n",
                i, clients[i].id, clients[i].name);
        #else //UDP
        fprintf(stderr, "client %d: type: %d name: %s\n",
                i, (int)clients[i].type, &clients[i].name[0]);
        #endif //TCP, UDP
    }
}

void unregister_client(int client_position){
    #ifdef TCP    
    IS_ERROR(epoll_ctl(epolld, EPOLL_CTL_DEL, clients[client_position].id, NULL),
             "delete descriptor from poll\n");
    #endif //TCP

    free(clients[client_position].name);
    clients[client_position] = clients[clients_in_array-1]; //move last client to free position
    clients_in_array--;
}

void *ping_function(void* arg){
    (void)arg;
    int ret;

    #ifdef TCP
    char message = CONTROL;
    #else //UDP
    message_struct message;
    message.type = CONTROL;
    #endif //TCP, UDP

    while(1){
        sleep(SLEEP_TIME);

        #ifdef DEBUG
        fprintf(stderr, "time to ping\n");
        #endif //DEBUG

        IS_ERROR(errno = pthread_mutex_lock(&client_mutex), "set mutex lock - pinger\n");
        for(int i=0; i<clients_in_array; i++){

            //sent control message
            #ifdef TCP
            ret = sendto(clients[i].id, &message, 1, MSG_NOSIGNAL, NULL, 0);
            #else //UDP
            ret = sendto((LOCAL == clients[i].type)? lsd: nsd,
                         &message, sizeof(message), MSG_NOSIGNAL,
                         (struct sockaddr*)&clients[i].address, clients[i].address_len);

            #endif //TCP, UDP
            if(ret == -1 && errno == EPIPE){ // if client disconnected
                #ifdef DEBUG
                fprintf(stderr, "client: %s disconnected\n",clients[i].name);
                #endif //DEBUG

                unregister_client(i);
                i--; //control moved client, so this position must be checked again
            }
        }
        IS_ERROR(errno = pthread_mutex_unlock(&client_mutex), "set mutex unlock - pinger\n");

        #ifdef DEBUG
        print_clients();
        #endif //DEBUG
    }
    return NULL;
}


void end_function(){
    pthread_cancel(socket_thread);
    pthread_cancel(pinger_thread);
    if(1 == exit_status){
        #ifdef DEBUG
        printf("closing network socket\n");
        #endif //DEBUG
        #ifdef TCP
        shutdown(nsd, SHUT_RDWR);
        #endif //TCP
        close(nsd);
    }
    #ifdef DEBUG
    printf("closing local socket\n");
    #endif //DEBUG
    #ifdef TCP
    shutdown(lsd, SHUT_RDWR);
    #endif //TCP
    close(lsd);

    #ifdef DEBUG
    printf("unlinking file socket\n");
    #endif //DEBUG
    unlink(pathname);
}

void end_signal(int sig){
    (void)sig;
    exit(EXIT_SUCCESS);
}
    
void create_local_socket(){
    IS_ERROR(lsd = socket(AF_UNIX, TYP, 0), "create local socket\n");
    struct sockaddr_un adr_un;
    memset(&adr_un, '\0', sizeof(struct sockaddr));
    adr_un.sun_family = AF_UNIX;
    strcpy(adr_un.sun_path, pathname);
    IS_ERROR(bind(lsd, (struct sockaddr*)&adr_un, sizeof(adr_un)), "bind local socket\n");

    #ifdef TCP
    IS_ERROR(listen(lsd, MAX_CLIENTS), "listen local socket\n");
    #endif //TCP
}

void set_at_exit(){
    if(atexit(end_function)){
        fprintf(stderr, "atexit error");
        exit(EXIT_FAILURE);
    }
}

void set_signal_handler(){
    struct sigaction act;
    act.sa_handler = end_signal;
    IS_ERROR(sigfillset(&act.sa_mask), "set full signal mask\n");
    act.sa_flags = 0;
    IS_ERROR(sigaction(SIGINT, &act, NULL), "set SIGINT handler\n");
}

void create_network_socket(){
    IS_ERROR(nsd = socket(AF_INET, TYP, 0), "create network socket\n");
    struct sockaddr_in adr_in;
    memset(&adr_in, '\0', sizeof(struct sockaddr));
    adr_in.sin_family = AF_INET;
    adr_in.sin_port = htobe16((uint16_t)port_number); //convert to big-endian
    adr_in.sin_addr.s_addr = INADDR_ANY;
    IS_ERROR(bind(nsd, (struct sockaddr*)&adr_in, sizeof(adr_in)), "bind network socket\n");

    #ifdef TCP
    IS_ERROR(listen(nsd, MAX_CLIENTS), "listen network socket\n");
    #endif //TCP
}

void create_epoll(){
    IS_ERROR(epolld = epoll_create1(0), "create epoll\n");/* 0 - no flags set*/
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; //read is avialible, repeat if not all readed
    event.data.fd = lsd;
    IS_ERROR(epoll_ctl(epolld, EPOLL_CTL_ADD, lsd, &event), "add local socket to poll\n");
    event.data.fd = nsd;
    IS_ERROR(epoll_ctl(epolld, EPOLL_CTL_ADD, nsd, &event), "add network socket to poll\n");
}

#ifdef TCP
void add_to_epoll(int descriptor){
    struct epoll_event event;
    event.data.fd = descriptor;
    event.events = EPOLLIN | EPOLLET;
    IS_ERROR(epoll_ctl(epolld, EPOLL_CTL_ADD, descriptor, &event),
        "add new descriptor to poll\n");
}

void register_client(int client_desc){
    char message, size, name_buf[CLIENT_MAX_NAME];
    if(clients_in_array == MAX_CLIENTS){
        message = NO_FREE_SLOT;                    
        send_all(client_desc, &message, 1, 0, NULL, 0);
        fprintf(stderr, "client connect, but there is no free slot\n");
        return;
    }

    int is_open = receive_all(client_desc, &size, 1, MSG_WAITALL, NULL, NULL);
    if(!is_open) return;
    #ifdef DEBUG
    fprintf(stderr, "recieve message with next message size\n");
    #endif //DEBUG

    is_open = receive_all(client_desc, &name_buf, size, MSG_WAITALL, NULL, NULL);
    if(!is_open) return;
    #ifdef DEBUG
    fprintf(stderr, "recieve message with client name %s\n", name_buf);
    #endif //DEBUG

    for(int i=0; i<clients_in_array; i++){
        if(0 == strcmp(clients[i].name, &name_buf[0])){ // name in use
            #ifdef DEBUG
            fprintf(stderr, "send response to client: name is use\n");
            #endif //DEBUG

            message = CLIENT_NAME_USED;
            send_all(client_desc, &message, 1, 0, NULL, 0);
            return;
        }
    }

    message = CLIENT_OK;
    send_all(client_desc, &message, 1, 0, NULL, 0); //send positive response to client
    int big_endian = htobe32(clients_in_array);
    send_all(client_desc, &big_endian, 1, 0, NULL, 0); //send client id

    #ifdef DEBUG
    fprintf(stderr, "send response to client\n");
    #endif //DEBUG

    clients[(int)clients_in_array].name = strdup(&name_buf[0]);
    clients[(int)clients_in_array++].id = client_desc;

    add_to_epoll(client_desc);
}

#else //UDP

void register_client(int desc, enum connection_type con_type, message_struct message,
                     struct sockaddr_storage src_addr, socklen_t sock_len){
    if(clients_in_array == MAX_CLIENTS){
        #ifdef DEBUG
        fprintf(stderr, "client connect, but there is no free slot\n");
        #endif //DEBUG

        message.type = NO_FREE_SLOT;
        send_all(desc, &message, sizeof(message), 0, (struct sockaddr*)&src_addr, sock_len);
        return;
    }

    for(int i=0; i<clients_in_array; i++){
        if(0 == strcmp(clients[i].name, message.data.name)){ // name in use
            #ifdef DEBUG            
            fprintf(stderr, "send response to client: name is use\n");
            #endif //DEBUG

            message.type = CLIENT_NAME_USED;
            send_all(desc, &message, sizeof(message), 0, (struct sockaddr*)& src_addr, sock_len);
            return;
        }
    }

    clients[(int)clients_in_array].name = strdup(&message.data.name[0]);
    clients[(int)clients_in_array].type = con_type;
    clients[(int)clients_in_array].address = src_addr;
    clients[(int)clients_in_array].address_len = sock_len;
    clients_in_array++;

    message.type = CLIENT_OK;
    send_all(desc, &message, sizeof(message), 0, (struct sockaddr*)&src_addr, sock_len);

    #ifdef DEBUG
    fprintf(stderr, "send response to client\n");
    #endif //DEBUG
}

#endif //TCP, UDP

expression_result berestoh(expression_result res){
    expression_result ret;
    ret.result = be32toh(res.result);
    ret.exp_id = be32toh(res.exp_id);
    return ret;
}

void *socket_function(void* arg){
    (void)arg;

    create_local_socket();
    set_at_exit();
    set_signal_handler();
    create_network_socket(); 
    exit_status++;
    create_epoll();
    
    //say reader, that he can start
    IS_ERROR(errno = pthread_mutex_unlock(&client_mutex), "set mutex unlock - configurator\n"); 
    IS_ERROR(errno = pthread_create(&pinger_thread, NULL, ping_function, NULL), 
             "create pinger thread\n");

    struct epoll_event event;
    int is_open;
    char message;

    #ifdef TCP
    int client_desc;

    #else //UDP
    message_struct messag;    
    struct sockaddr_storage address;
    socklen_t addr_len, sockaddr_len = (socklen_t)sizeof(struct sockaddr_storage);
    #endif //TCP, UDP

    while(1){
        #ifdef DEBUG
        fprintf(stderr, "waiting for message\n");
        #endif //DEBUG

        IS_ERROR(epoll_wait(epolld, &event, 1, -1), "epoll noticed event\n");

        #ifdef TCP
        if(event.data.fd == lsd || event.data.fd == nsd){
            IS_ERROR(client_desc = accept(event.data.fd, NULL, NULL), "accept connection\n");
        }else{
            client_desc = event.data.fd;
        }
        //receive type message
        is_open = receive_all(client_desc, &message, 1, MSG_WAITALL, NULL, NULL);

        #else //UDP
        addr_len = sockaddr_len;
        is_open = receive_all(event.data.fd, &messag, sizeof(messag), MSG_WAITALL,
                              (struct sockaddr *)&address, &addr_len);
        message = messag.type;
        #endif //TCP, UDP

        if(!is_open) continue;
        IS_ERROR(errno = pthread_mutex_lock(&client_mutex), "set mutex lock - receiver\n");
        switch(message){
            case REGISTER:{
                #ifdef TCP
                register_client(client_desc);
                #else //UDP
                register_client(event.data.fd, (lsd == event.data.fd)? LOCAL: NETWORK, messag,
                                address, addr_len);
                #endif //TCP, UDP
                break;
            }
            case COMPUTED_RESULT:{
                expression_result result;
                #ifdef TCP
                is_open = receive_all(client_desc, &result, sizeof(result), 
                                      MSG_WAITALL, NULL, NULL);
                if(!is_open){
                    fprintf(stderr, "error - client disconnected\n");
                    break;
                }
                result = berestoh(result);
                #else //UDP
                result = berestoh(messag.data.result);
                #endif //TCP, UDP

                printf("expression id: %d, result: %d\n", result.exp_id, result.result);
                break;
            }
            case UNREGISTER:{
                for(int i=0; i<clients_in_array; i++){
                    #ifdef TCP
                    if(client_desc == clients[i].id){
                    #else //UDP
                    if(0 == strcmp(&messag.data.name[0], clients[i].name)){
                    #endif //TCP, UDP

                        #ifdef DEBUG
                        fprintf(stderr, "client %s unregistered\n", clients[i].name);
                        #endif

                        unregister_client(i);
                        break;
                    }
                }
                break;
            }
            default:{
                fprintf(stderr, "unknown message\n");
            }
        }//end switch 
        IS_ERROR(errno = pthread_mutex_unlock(&client_mutex), "set mutex unlock - receiver\n");
    }
    return NULL;
}

client_info choose_client(){
    IS_ERROR(errno = pthread_mutex_unlock(&client_mutex), "set mutex lock - chooser\n");
    static char last_choose = 0;
    if(++last_choose >= clients_in_array){
        last_choose = 0;
    }
    client_info ret = clients[(int)last_choose];
    IS_ERROR(errno = pthread_mutex_unlock(&client_mutex), "set mutex unlock - chooser\n");
    return ret;
}

expression get_expression_in_be(char op, int a, int b, int id){
    expression ex;
    ex.op = op;
    ex.numA = htobe32(a);
    ex.numB = htobe32(b);
    ex.exp_id = htobe32(id);
    return ex;
}

void reader(){
    int b,a,tmp;
    char op[2];

    #ifdef TCP
    char message = ORDER;
    #else //UDP

    message_struct message;
    message.type = ORDER;
    #endif //TCP, UDP

    while(1){
        tmp = scanf("%d%*[ ]%1[+-*/]%*[ ]%d", &a, &op[0], &b); //read input
        if(tmp == 1)tmp += scanf("%1[+-*/]%*[ ]%d", &op[0], &b); //if there no spaces try it
        if(tmp == 2)tmp += scanf("%d", &b); //if there no spaces try it
        if(tmp != 3){
            printf("incorret input: %c\n", getchar());
        }else if(clients_in_array == 0){
            printf("clients are not connected\n");
            continue;
        }else{
            expression ex = get_expression_in_be(op[0], a, b, order_id++);
            client_info cl = choose_client();
      
            #ifdef DEBUG
            fprintf(stderr, "send request to client: %s, to compute: %d %c %d,"
                    " expression id: %d\n", cl.name, a, op[0], b, ex.exp_id);
            #endif

            #ifdef TCP
            send_all(cl.id, &message, 1, 0, NULL, 0); //send type message
            send_all(cl.id, &ex, sizeof(ex), 0, NULL, 0); //send expression message

            #else //UDP
            message.data.exp = ex;
            send_all((cl.type == LOCAL)? lsd: nsd, &message, sizeof(message), 0,
                     (struct sockaddr *)&cl.address, cl.address_len);
            #endif //TCP, UDP
        }
    }
}

int main(int argc, char* argv[]){
    parse_arg(argc, argv);
    
    //can't use client while connection isn't configurated
    errno = pthread_mutex_lock(&client_mutex);
    IS_ERROR((errno)? -1: 0, "set mutex lock - configurator\n");  
    errno = pthread_create(&socket_thread, NULL, socket_function, NULL);
    IS_ERROR((errno)? -1: 0, "create socket thread\n");
    
    //if mutex will be unlocked, connection is ready
    errno = pthread_mutex_lock(&client_mutex);
    IS_ERROR((errno)? -1: 0, "set mutex lock - reader\n");
    errno = pthread_mutex_unlock(&client_mutex);
    IS_ERROR((errno)? -1: 0, "set mutex unlock - reader\n");

    reader();
}

