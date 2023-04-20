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

void send_to_other_clients(Node* head, int socketClient, char* message) {
  Node* currentClient = head->next;
  while(currentClient != head) {
    if(currentClient->number != socketClient) {
      if(send(currentClient->number, message, strlen(message)+1, 0) == -1) {
        perror("Erreur: Envoi du message");
        exit(1);
      }
      printf("Envoi du message au client %d\n", currentClient->number);
    }
    currentClient = currentClient->next;
  }
}

void* thread_client(void* args) {
  ThreadArgs* data = (ThreadArgs*) args;
  int socketClient = data->socketClient;
  Node* listClient = data->listClient;

  int nbOctetsLu = 1;
  while(nbOctetsLu != 0 || nbOctetsLu != -1) {

    /**** Receive message from client ****/
    char message[NB_CARACTERES];
    int nbOctetsLu = recv(socketClient, message, NB_CARACTERES, 0);
    if(nbOctetsLu == -1) {
      perror("Erreur: réception du message");
      close_client(socketClient);
    } else if(nbOctetsLu == 0) {
      printf("La socket a été fermée par le client\n");
      close_client(socketClient);
    } else {
      printf("Message reçu : %s\n", message);

      /**** Send message to other clients ****/
      send_to_other_clients(listClient, socketClient, message);
    }
  }

  pthread_exit(0);
}

void close_client(int socketClient) {
  if(shutdown(socketClient, 2)) {
    perror("Error: Close socket");
    exit(1);
  }
  printf("Socket %d client closed\n");
}