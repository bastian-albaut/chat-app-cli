#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "../include/list.h"
#include "../include/constants.h"
#include "../include/server_functions.h"
#include "../include/global.h"

int main(int argc, char *argv[]) {

  /**** Arguments verification ****/
  if(argc != 2){
    perror("Error: Expected argument is <port>");
    exit(1);
  }


  /**** Creation of socket ****/
	socketServer = socket(PF_INET, SOCK_STREAM, 0);
	if(socketServer == -1) {
    perror("Error: Creation of socket");
  }
  printf("Socket Created\n");


  /**** Socket naming ****/
	struct sockaddr_in ad;
	ad.sin_family = AF_INET; 
	ad.sin_addr.s_addr = INADDR_ANY;
	ad.sin_port = htons(atoi(argv[1]));
	if(bind(socketServer, (struct sockaddr*)&ad, sizeof(ad)) == -1) {
    perror("Error: Socket naming");
    exit(1);
  }
  printf("Named Socket successful\n");


  /**** Putting the socket in listening mode ****/
	if(listen(socketServer, 10) == -1) {
		perror("Error: Putting the socket in listening mode");
	}
  printf("Listen mode\n");


  /**** Initialize the clients list ****/
  listClient = NULL;
  init_head(&listClient);


  /**** Prepare acceptation of client ****/
  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);
  int socketClient = -1;

  /**** Catch the SIGINT signal ****/
  signal(SIGINT, interrupt_handler);

  while(1) {

    /**** Accept a client connection ****/
    socketClient = accept(socketServer, (struct sockaddr*) &aC,&lg) ;
    if(socketClient == -1) {
      perror("Error: Acceptance of client connection");
    }

    /**** Add the client to the list ****/
    Node* currentClient = insert_first(&listClient, socketClient);
    printf("Client Connected !\n");
    display_list(&listClient);

    /**** Create a thread for the client ****/
    ThreadArgs args = {currentClient->number, listClient};
    pthread_create(&(currentClient->thread), NULL, thread_client, &args);

  }

  /**** Closing the socket ****/
  close_socket(socketServer);

  printf("End of program\n");
}