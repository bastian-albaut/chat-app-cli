#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NB_CARACTERES 32


/**** Variables globales ****/
char chaine[NB_CARACTERES];


/**** Prototype des fonctions ****/
void * ThreadEnvoi(void *descripteurServeur);


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
  pthread_create(&thread, NULL, ThreadEnvoi, &descripteurServeur);

  while(1) {  

    /**** Reception du message du serveur ****/
    int r;
    printf("En attente de réception d'un message...\n");
    int nbOctetsLu = recv(descripteurServeur, &r, sizeof(r), 0);
    if(nbOctetsLu == -1) {
      perror("Erreur: réception du message");
      exit(1);
    } else if(nbOctetsLu == 0) {
      printf("Erreur: La socket a été fermée");
      break;
    }
    printf("Réponse reçue : %d\n", r);

  }

  /**** Fermeture de la socket ****/
  shutdown(descripteurServeur,2);

  printf("Fin du programme\n");
}



void * ThreadEnvoi(void *descripteurServeur) {
  int *socketServeur = (int*) descripteurServeur;

  while(1) {

    /**** Préparation à l'envoi du message au serveur ****/
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