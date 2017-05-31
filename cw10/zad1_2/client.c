#include "const.h"

const char* client_name;
enum connection_type con;
const char* pathname;
struct sockaddr_un adr_un;
struct sockaddr_in adr_in;

char client_id;

void parse_arg(int argc, char* argv[]){
    while(argc >= 4){
        if(strlen(argv[1]) + 1 > CLIENT_MAX_NAME) break; //wrong input
        client_name = argv[1];

        int t = strtol(argv[2], NULL, 10);
        switch(t){
            case 0:{ /* chose local connection*/
                con = LOCAL;
                if(strlen(argv[3]) + 1 > MAX_PATH_SIZE){ //string + null
                    fprintf(stderr, "socket_name is too long\n");
                    exit(EXIT_FAILURE);
                }
                pathname = argv[3];
                adr_un.sun_family = AF_UNIX;
                strcpy(adr_un.sun_path, pathname);
                return;
            }
            case 1:{ /*chose network connection */
                if(argc != 5) break;
                int port_number = strtol(argv[4], NULL, 10);
                if(port_number < 1024 || port_number > 65535){
                    break;// below 1024 need additional permission, max is 65535
                }
                con = NETWORK;
                if(0 == inet_pton(AF_INET, argv[3], &adr_in)){
                    fprintf(stderr, "socket adres invalid, corret format is \"X.X.X.X\"\n");
                    exit(EXIT_FAILURE);
                }
                adr_in.sin_family = AF_INET;
                adr_in.sin_port = htobe16((uint16_t)port_number);
                return;
            }
            default:{ 
                //wrong input
            }
        }
        break;
    }

    fprintf(stderr, "give 3 or 4 args:\n\
\t1)client_name{max_size=%d}\n\
\t2)connection_type{0 - local, 1 - network}\n\
if connection_type == network then:\n\
\t3)adresIPv4{X.X.X.X}\n\
\t4)"PROTOCOL"_port_number{1024..65535}\n\
else:\n\
\t3)socket_name\n", CLIENT_MAX_NAME);

    exit(EXIT_FAILURE);
}

int sd; //socket descriptor
void end_function(){
    #ifdef TCP
    shutdown(sd, SHUT_RDWR);
    #endif //TCP

    close(sd);
}

void end_signal(int sig){
    (void)sig;

    #ifdef TCP
    char message = UNREGISTER;
    send_all(sd, &message, 1, 0, NULL, 0);

    #else //UDP
    message_struct message;
    message.type = UNREGISTER;
    strcpy(&message.data.name[0], client_name);
    send_all(sd, &message, sizeof(message), 0, (con == LOCAL)? (struct sockaddr*)&adr_un:
             (struct sockaddr*)&adr_in, sizeof(struct sockaddr));
    #endif //TCP, UDP    

    exit(EXIT_SUCCESS);
}

void set_at_exit(){
    if(atexit(end_function)){
        end_function();
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

void connect_to_server(){
    IS_ERROR(sd = socket((con == LOCAL)? AF_UNIX: AF_INET, TYP, 0), 
             "create %s socket\n" COMMA (con == LOCAL)? "local": "network");
    #ifdef UDP    
    if(con == LOCAL){
        IS_ERROR(bind(sd, (struct sockaddr *)&adr_un,
                      sizeof(((struct sockaddr*)0)->sa_family)), "bind success\n");
    }
    #endif //UDP
    IS_ERROR(connect(sd, ((con == LOCAL)? (struct sockaddr *)&adr_un: (struct sockaddr *)&adr_in),
                     sizeof(struct sockaddr)), "connect to server\n");
    set_at_exit();    
    set_signal_handler();   

    #ifdef TCP
    char message = REGISTER;
    send_all(sd, &message, 1, 0, NULL, 0); //send type message
    message = strlen(client_name) + 1;
    send_all(sd, &message, 1, 0, NULL, 0); //send length message - string + null
    send_all(sd, client_name, strlen(client_name) + 1, 0, NULL, 0); //send name
    receive_all(sd, &message, 1, 0, NULL, NULL);

    #else //UDP
    message_struct messag;
    messag.type = REGISTER;
    strcpy(&messag.data.name[0], client_name);
    send_all(sd, &messag, sizeof(messag), 0, NULL, 0);
    receive_all(sd, &messag, sizeof(messag), 0, NULL, NULL);
    char message = messag.type;
    #endif //TCP, UDP

    switch(message){
        case CLIENT_NAME_USED:{
            printf("name \"%s\" already in use\n", client_name);
            exit(EXIT_SUCCESS);
        }
        case NO_FREE_SLOT:{
            printf("server have maximum number of clients\n");
            exit(EXIT_SUCCESS);
        }
        case CLIENT_OK:{
            #ifdef TCP
            receive_all(sd, &client_id, 1, 0, NULL, NULL);
            #endif //TCP

            #ifdef DEBUG
            fprintf(stderr, "client connect successful\n");
            #endif //DEBUG

            break;
        }
        default:{
            printf("unknown message\n");
            exit(EXIT_FAILURE);
        }
    }
}

expression_result compute(expression ex){
    expression_result ret;
    ret.exp_id = ex.exp_id;
    switch(ex.op){
        case '+': return (ret.result = ex.numA + ex.numB, ret);
        case '-': return (ret.result = ex.numA - ex.numB, ret);
        case '*': return (ret.result = ex.numA * ex.numB, ret);
        case '/': return (ret.result = (!ex.numB)? fprintf(stderr, "div by 0!\n"), 0: 
                                      (ex.numA / ex.numB), ret);
        default: {
            fprintf(stderr, "invalid operation\n");
            ret.result = 0;
            return ret;
        }
    }
}

expression beextoh(expression ex){
    expression exp;
    exp.op = ex.op;
    exp.numA = be32toh(ex.numA);
    exp.numB = be32toh(ex.numB);
    exp.exp_id = be32toh(ex.exp_id);
    return exp;
}

expression_result htoberes(expression_result res){
    expression_result ret;
    ret.result = htobe32(res.result);
    ret.exp_id = htobe32(res.exp_id);
    return ret;
}


void start_work(){
    int is_open;
    while(1){
        #ifdef DEBUG
        fprintf(stderr, "waiting for orders\n");
        #endif //DEBUG

        #ifdef TCP
        char message;  // receive type message
        is_open = receive_all(sd, &message, 1, 0, NULL, NULL);

        #else //UDP
        message_struct messag;
        is_open = receive_all(sd, &messag, sizeof(messag), 0, NULL, NULL);
        char message = messag.type;
        #endif //TCP, UDP

        if(!is_open){
            exit(EXIT_FAILURE);
        }
        switch(message){
            case CONTROL:{
                break; //ignore
            }
            case ORDER:{
                #ifdef TCP
                receive_all(sd, &ex, sizeof(ex), 0, NULL, NULL); //receive experssion message
                expression ex = beextoh(ex); //big endian experssion to host
                #else //UDP
                expression ex = beextoh(messag.data.exp);
                #endif //TCP, UDP

                expression_result result = compute(ex);
                #ifdef DEBUG
                fprintf(stderr, "order id: %d, input: %d %c %d, output: %d\n",
                        ex.exp_id, ex.numA, ex.op, ex.numB, result.result);
                #endif //DEBUG

                #ifdef TCP
                message = COMPUTED_RESULT;
                result = htoberes(result); //host experssion to big endain 
                send_all(sd, &message, 1, 0, NULL, 0); //send type message
                send_all(sd, &result, sizeof(result), 0, NULL, 0); //send computed result 
                #else //UDP
                messag.type = COMPUTED_RESULT;
                messag.data.result = htoberes(result); //host experssion to big endain 
                send_all(sd, &messag, sizeof(messag), 0, NULL, 0); 
                #endif //TCP, UDP

                break;
            }
            default:{
                fprintf(stderr, "unknown message\n");
                exit(EXIT_FAILURE);
            }
        }//switch
    }
}

int main(int argc, char* argv[]){
    parse_arg(argc, argv);
    connect_to_server();
    start_work();
}

