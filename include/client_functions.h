#ifndef CLIENT_FUNCTIONS_H
#define CLIENT_FUNCTIONS_H

extern int socketServerFromClient;

extern void* thread_send(void *socket);
extern void interrupt_handler(int signal);

#endif // CLIENT_FUNCTIONS_H