#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/list.h"
#include "../include/constants.h"
#include "../include/server_functions.h"

int main(int argc, char *argv[]) {
  
  if(argc != 2){
    perror("Argument attendu : port");
    exit(1);
  }

  /**** Création de la socket ****/
	// Création de la socket avec le domaine de communication "IPv4" 
  // et le type de format "Stream" en utilisant le protocole TCP
	int descripteurServeur = socket(PF_INET, SOCK_STREAM, 0);
	if(descripteurServeur == -1) {
    perror("Erreur: Création de la socket");
  }
  printf("Socket Créé\n");


  /**** Nommage de la socket ****/
	struct sockaddr_in ad;
	ad.sin_family = AF_INET; // Domaine de communication "IPv4"
	ad.sin_addr.s_addr = INADDR_ANY; // Attache le socket à toutes les interfaces réseaux locales
	ad.sin_port = htons(atoi(argv[1])); // Port d'écoute
	if(bind(descripteurServeur, (struct sockaddr*)&ad, sizeof(ad)) == -1) {
    perror("Erreur: Nommage de la socket");
    exit(1);
  }
  printf("Socket Nommé\n");


  /**** Passage de la socket en mode écoute ****/
	// Avec 10 demandes de connexions maximum en fil d'attente
	if(listen(descripteurServeur, 10) == -1) {
		perror("Erreur: Passage de la socket en mode écoute");
	}
  printf("Mode écoute\n");


  /**** Initialize the client list ****/
  Node* listClient = NULL;
  init_head(&listClient);

  /**** Prepare acceptation of client ****/
  struct sockaddr_in aC;
  socklen_t lg = sizeof(struct sockaddr_in);
  int descripteurClient = -1;

  while(1) {

    /**** Accept a client connection ****/
    descripteurClient = accept(descripteurServeur, (struct sockaddr*) &aC,&lg) ;
    if(descripteurClient == -1) {
      perror("Erreur: Acceptation de la connexion du client");
    }

    /**** Add the client to the list ****/
    Node* currentClient = insert_first(&listClient, descripteurClient);
    printf("Client Connecté\n");
    display_list(&listClient);

    /**** Create a thread for the client ****/
    ThreadArgs args = {currentClient->number, listClient};
    pthread_create(&(currentClient->thread), NULL, thread_client, &args);

  }

  /**** Fermeture des sockets ****/
  shutdown(descripteurClient, 2); 
  shutdown(descripteurServeur, 2);

  printf("Fin du programme\n");
}