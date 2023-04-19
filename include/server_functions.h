#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "list.h"

typedef struct ThreadArgs {
    int socketClient;
    void* listClient;
} ThreadArgs;


extern void* thread_client(void *dSClient);
extern void send_to_other_clients(Node* listClient, int socketClient, char* message);

#endif // SERVER_FUNCTIONS_H
