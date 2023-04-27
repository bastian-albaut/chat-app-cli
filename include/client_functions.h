#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

extern int socketServerFromClient;

extern void* thread_send(void *socket);
void send_pseudo();
extern void interrupt_handler(int signal);
extern void get_input(char* message, int size, char* prompt);

#endif // CLIENT_FUNCTIONS_H