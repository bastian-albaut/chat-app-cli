#ifndef GLOBAL_H
#define GLOBAL_H

#include "list.h"

extern int socketServer;
extern Node* listClient;

extern void close_socket(int socket);
extern void send_message(int socket, char* message, char* prompt);

#endif // GLOBAL_H
