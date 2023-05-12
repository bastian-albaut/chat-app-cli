#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "../include/client_functions.h"
#include "../include/constants.h"
#include "../include/global.h"


/**
 * Main function of the client
 *
 * @param argc Number of arguments
 * @param argv Array of arguments
 *
 * @return int
 */
int main(int argc, char *argv[]) {

  // Arguments verification
  if(argc != 3){
    perror("Error: Expected arguments are <ip> <port>");
    exit(1);
  }

  // Semaphore initialization and taking place
  init_semaphore_client();
  take_place_semaphore();

  // Socket initialization
  init_socket_client();

  // Server connection request
  connection_request(argv[1], argv[2]);

  // Catch the SIGINT signal
  signal(SIGINT, interrupt_handler);

  // Send pseudo to the server
  send_pseudo();

  // Creation of a thread for sending message(s) to the server
  pthread_t thread;
  pthread_create(&thread, NULL, thread_send, &socketServerFromClient);

  // Loop to receive message(s) from the server
  while(1) {  

    // Receive message from server
    char *message = malloc(NB_CHARACTERS * sizeof(char));

    Response* response = malloc(sizeof(Response));
    int nbByteRead = recv_response(socketServerFromClient, response);

    if(nbByteRead == 0 || nbByteRead == -1) {
      printf("The connection was cut on the server side\n");
      break;
    }

    print_response(response);
  }

  interrupt_handler(0);
}