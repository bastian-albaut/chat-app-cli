#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "list.h"
#include "global.h"

typedef struct ThreadArgs {
    int socketClient;
    void* listClient;
} ThreadArgs;


extern void* thread_client(void* args);
extern int handle_private_message(char* message, int socketClient, char* pseudoTransmitter);
extern int is_private_message(char* message);
extern char* get_pseudo_private_message(char* message);
extern char* get_content_private_message(char* message);
extern int send_private_message(Node* head, char* pseudo, char* message, char* pseudoTransmitter);
extern void handle_global_message(char* message, int socketClient, char* pseudoTransmitter);
extern int send_to_other_clients(char* message, int socketClient, char* pseudoTransmitter);
extern char* get_pseudo(int socketClient);
extern void interrupt_handler(int signal);
extern void close_all_clients();
extern void remove_client(int socketClient);
extern int recv_message(int socket, char* message);
extern int send_response(int socketClient, int code, char* message, char* pseudo);
extern void serialize_response(Response* response, char* buffer, size_t sizeBuffer);

#endif // SERVER_FUNCTIONS_H
