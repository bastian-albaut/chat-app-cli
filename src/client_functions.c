#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/client_functions.h"
#include "../include/constants.h"

void * thread_send(void *descripteurServeur) {
  int *socketServeur = (int*) descripteurServeur;

  while(1) {

    /**** Préparation à l'envoi du message au serveur ****/
    char chaine[NB_CARACTERES];
    printf("Veuillez entrer le message que vous souhaitez envoyer:\n");
    fgets(chaine, NB_CARACTERES, stdin);
    char *findReturn = strchr(chaine,'\n'); // Renvoie null si pas trouvé
    if(findReturn != NULL) {
      *findReturn = '\0';
    }

    /**** Envoi du message au serveur ****/
    if(send(*socketServeur, chaine, strlen(chaine)+1, 0) == -1){
      perror("Erreur envoie du message");
      exit(1);
    }
    printf("Message envoyé au serveur\n");

  }
  pthread_exit(0);
}