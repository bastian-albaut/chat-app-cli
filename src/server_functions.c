#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/server_functions.h"
#include "../include/list.h"
#include "../include/constants.h"
#include "../include/global.h"

/**** Send message to other clients ****/
void send_to_other_clients(Node* head, int socketClient, char* message) {
  Node* currentClient = head->next;
  while(currentClient != head) {
    if(currentClient->number != socketClient) {
      if(send(currentClient->number, message, strlen(message)+1, 0) == -1) {
        perror("Error: Sending the message");
        exit(1);
      }
      printf("Message send to the client %d\n", currentClient->number);
    }
    currentClient = currentClient->next;
  }
}

/**** Thread for each client ****/
void* thread_client(void* args) {
  ThreadArgs* data = (ThreadArgs*) args;
  int socketClient = data->socketClient;
  Node* listClient = data->listClient;

  int nbByteRead = 1;
  while(nbByteRead != 0 && nbByteRead != -1) {

    /**** Receive message from client ****/
    char message[NB_CHARACTERS];
    nbByteRead = recv(socketClient, message, NB_CHARACTERS, 0);
    if(nbByteRead == -1) {
      perror("Error: Receiving the message");
      remove_client(socketClient);
    } else if(nbByteRead == 0) {
      remove_client(socketClient);
    } else {
      printf("Message receive: %s\n", message);

      /**** Send message to other clients ****/
      send_to_other_clients(listClient, socketClient, message);
    }
  }

  pthread_exit(0);
}

void interrupt_handler(int signal) {
  printf("\n");
  close_all_clients();
  printf("  =>  All client threads/sockets are closed\n");
  close_socket(socketServer);
  printf("  =>  Server socket is closed\n");
  printf("\n========== END OF SERVER ==========\n");
  exit(0);
}

/**** Close all clients thread/sockets in the list ****/
void close_all_clients() {
  Node* currentClient = listClient->next;
  while(currentClient != listClient) {

    /**** Close thread ****/
    pthread_cancel(currentClient->thread);

    /**** Close socket ****/
    close_socket(currentClient->number);

    currentClient = currentClient->next;
  }
}

/**** Remove the client from the list and close the socket corresponding ****/
void remove_client(int socketClient) {
  remove_element(&listClient, search_element(&listClient, socketClient));
  printf("Client %d disconnected\n", socketClient);
  display_list(&listClient);
  close_socket(socketClient);
}