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
extern void handle_message(char* message, int socketServer);
extern int is_list_file_message(char* message);
extern void handle_list_file_message(char* message);
extern int is_good_format_list_file_message(char* message);
extern int is_send_file_message(char* message);
extern void handle_send_file_message(char* message, int socketServer);
extern int is_good_format_send_file_message(char* message);
extern char* get_file_name(char* message);
extern int is_file_exist(char* fileName);
extern void send_file(int socketServer, char* fileName);
extern void* thread_file_transfer(void *arg);
extern int is_recv_file_message(char* message);
extern int init_socket_file();
extern void name_socket_file(int socketFile, int port);
extern void listen_socket_file(int socketFile);

#endif // CLIENT_FUNCTIONS_H