#ifndef GLOBAL_H
#define GLOBAL_H

#include "list.h"

extern int socketServer;
extern Node* listClient;

extern void close_socket(int socket);
extern void send_message(int socket, char* message, char* prompt);
extern int recv_message(int socket, char* message);
extern void get_input(char* message, int size, char* prompt);

#endif // GLOBAL_H
