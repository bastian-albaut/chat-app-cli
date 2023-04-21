#ifndef GLOBAL_H
#define GLOBAL_H

#include "list.h"

extern int socketServer;
extern Node* listClient;

extern void close_socket(int socket);

#endif // GLOBAL_H
