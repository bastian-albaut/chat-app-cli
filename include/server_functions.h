#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "list.h"
#include "global.h"

typedef struct ThreadArgs {
    pthread_t* ptrTempIdThread;
    int socketClient;
} ThreadArgs;

typedef struct ThreadArgsFile {
    FILE* file;
    int socketFile;
    int sizeFile;
} ThreadArgsFile;

union semun{
    int val;
    struct semid_ds * buff;
    short * array;
};

extern int idSemaphore;

extern void* thread_client(void* args);
extern void init_socket_server();
extern void name_socket_server(char* port);
extern void listen_socket_server();
extern int accept_client();
extern int handle_pseudo_client(int socketClient, char* pseudo);
extern char* get_pseudo(int socketClient);
extern int check_format_pseudo(char* pseudo, char* responseMessage);
extern void handle_message(char* message, int socketClient, char* pseudo, pthread_t threadId);
extern char is_special_command(char* message);
extern int is_private_message(char* message);
extern int is_global_message(char* message);
extern int is_logout_message(char* message);
extern int is_help_message(char* message);
extern int is_send_file_message(char* message);
extern void handle_private_message(char* message, int socketClient, char* pseudoTransmitter);
extern int is_good_format_private_message(char* message);
extern char* get_pseudo_private_message(char* message);
extern char* get_content_private_message(char* message);
extern int send_private_message(Node* head, char* pseudo, char* message, char* pseudoTransmitter);
extern int is_good_format_global_message(char* message);
extern char* get_content_global_message(char* message);
extern void handle_global_message(char* message, int socketClient, char* pseudoTransmitter);
extern int send_to_other_clients(char* message, int socketClient, char* pseudoTransmitter);
extern void handle_logout_message(char* message, int socketClient, pthread_t threadId);
extern int is_good_format_logout_message(char* message);
extern void handle_help_message(char* message, int socketClient);
extern int is_good_format_help_message(char* message);
extern char* get_content_of_file(char* filename);
extern void handle_send_file_message(char* message, int socketClient);
extern int is_file_name_and_size_ok(char* fileName, int size, char* message);
extern int is_good_format_send_file_message(char* message);
extern void get_file_name_and_size(char* message, char** file_name, int* file_size);
extern FILE* create_file(char* file_name);
extern int init_socket_file();
extern void connection_request(int socketFile, int port);
extern void* thread_receive_file(void* args);
extern void interrupt_handler(int signal);
extern void close_all_clients();
extern void remove_client(int socketClient, pthread_t threadId);
extern int recv_message(int socket, char* message);
extern int send_response(int socketClient, int code, char* message, char* pseudo);
extern void serialize_response(Response* response, char* buffer, size_t sizeBuffer);
extern void init_semaphore_server();
extern void set_capacity_semaphore(int capacity);
extern void deletion_semaphore();
extern void leave_place_semaphore();
void display_remaining_places_semaphore();

#endif // SERVER_FUNCTIONS_H
