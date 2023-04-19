#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../include/client_functions.h"
#include "../include/constants.h"

int main(int argc, char *argv[]) {

  /**** Vérification des arguments ****/
  if(argc != 3){
    perror("Argument attendu : 127.0.0.1 port");
    exit(1);
  }

  /**** Création de la socket ****/
  // Création de la socket avec le domaine de communication "IPv4" 
  // et le type de format "Stream" en utilisant le protocole TCP
  int descripteurServeur = socket(PF_INET, SOCK_STREAM, 0);
  if(descripteurServeur == -1) {
    perror("Erreur: Création de la socket");
  }

  /**** Demande de connexion au serveur ****/
  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr));
  aS.sin_port = htons(atoi(argv[2]));
  socklen_t lgA = sizeof(struct sockaddr_in);
  
  if(connect(descripteurServeur, (struct sockaddr *) &aS, lgA) == -1){
    perror("Erreur: Demande de connexion au serveur");
    exit(1);
  }
  printf("Socket Connecté au serveur\n");

  /**** Création d'un thread pour l'envoi de message(s) au serveur ****/
  pthread_t thread;
  pthread_create(&thread, NULL, thread_send, &descripteurServeur);

  int nbOctetsLu = 1
  while(nbOctetsLu != 0) {  

    /**** Reception du message du serveur ****/
    char *message = malloc(NB_CARACTERES * sizeof(char));
    printf("En attente de réception d'un message...\n");
    nbOctetsLu = recv(descripteurServeur, message, NB_CARACTERES, 0);
    if(nbOctetsLu == -1) {
      perror("Erreur: réception du message");
      exit(1);
    } else if(nbOctetsLu == 0) {
      printf("Erreur: La socket a été fermée");
    } else {
      printf("Réponse reçue : %s\n", message);
    }
  }

  /**** Fermeture de la socket ****/
  shutdown(descripteurServeur,2);

  printf("Fin du programme\n");
}