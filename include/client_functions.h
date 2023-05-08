#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

#include "../include/global.h"

extern int socketServerFromClient;
extern int idSemaphore;

extern void* thread_send(void *socket);
extern void init_socket_client();
extern void connection_request(char* ipAdress, char* port);
extern void send_pseudo();
extern void interrupt_handler(int signal);
extern void get_input(char* message, int size, char* prompt);
extern void send_message(int socket, char* message, char* prompt);
extern int recv_response(int socketServer, Response* response);
extern void deserialize_response(char* buffer, size_t sizeBuffer, Response* response);
extern void print_response(Response* response);
extern void init_semaphore_client();
extern void take_place_semaphore();

#endif // CLIENT_FUNCTIONS_H