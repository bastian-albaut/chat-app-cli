#include "../include/list/list.h"
#include "../include/constants.h"
#include "../include/server/global_server.h"
#include "../include/server/semaphore/semaphore_server.h"
#include "../include/global.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>


int main(int argc, char *argv[]) {

  // Arguments verification
  if(argc != 2){
    perror("Error: Expected argument is <port>");
    exit(1);
  }

  // Socket initialization
  init_socket(&socketServer, 1, 1);

  // Socket naming
  name_socket(&socketServer, atoi(argv[1]), 1);
  
  // Socket listening
  listen_socket_server();

  // Initialize the clients list
  listClient = NULL;
  init_head(&listClient);

  // Initialize the semaphore
  deletion_semaphore();
  init_semaphore_server();
  set_capacity_semaphore(CAPACITY_CLIENTS);

  // Catch the SIGINT signal
  signal(SIGINT, interrupt_handler);

  // Loop to accept clients and create threads for them
  while(1) {

    int socketClient = accept_client();
    if(socketClient == -1) {
      perror("Error: Acceptation of client");
      break;
    }

    // Display the number of remaining places
    display_remaining_places_semaphore();

    // Create a thread for the client
    pthread_t* ptrTempIdThread = malloc(sizeof(pthread_t));
    ThreadArgs args = {ptrTempIdThread, socketClient};
    pthread_create(ptrTempIdThread, NULL, thread_client, &args);

  }
  interrupt_handler(1);
}