#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "list.h"

typedef struct ThreadArgs {
    int socketClient;
    void* listClient;
} ThreadArgs;


extern void* thread_client(void* args);
extern void send_to_other_clients(Node* head, int socketClient, char* message);
extern void interrupt_handler(int signal);
extern void close_all_clients_sockets();
extern void remove_client(int socketClient);

#endif // SERVER_FUNCTIONS_H
