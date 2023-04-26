#ifndef SERVER_FUNCTIONS_H
#define SERVER_FUNCTIONS_H

#include "list.h"

typedef struct ThreadArgs {
    int socketClient;
    void* listClient;
} ThreadArgs;


extern void* thread_client(void* args);
extern int handle_private_message(char* message, int socketClient);
extern int is_private_message(char* message);
extern char* get_pseudo_private_message(char* message);
extern char* get_content_private_message(char* message);
extern int send_private_message(Node* head, char* pseudo, char* message);
extern void send_to_other_clients(Node* head, int socketClient, char* message);
extern char* get_pseudo(int socketClient);
extern void interrupt_handler(int signal);
extern void close_all_clients();
extern void remove_client(int socketClient);

#endif // SERVER_FUNCTIONS_H
