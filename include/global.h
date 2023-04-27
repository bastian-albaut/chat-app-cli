#ifndef GLOBAL_H
#define GLOBAL_H

#include "list.h"
#include "constants.h"

typedef struct Response {
  int code;
  char* message;
  char* from;
} Response;

extern int socketServer;
extern Node* listClient;

extern void close_socket(int socket);

#endif // GLOBAL_H
