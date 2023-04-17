#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define NB_CARACTERES 32

int main(int argc, char *argv[]) {

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
  inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
  aS.sin_port = htons(atoi(argv[2])) ;
  socklen_t lgA = sizeof(struct sockaddr_in) ;
  
  if(connect(descripteurServeur, (struct sockaddr *) &aS, lgA) == -1){
    perror("Erreur: Demande de connexion au serveur");
    exit(1);
  }
  printf("Socket Connecté au serveur\n");

  for(int i=0; i<2; i++) {
    /**** Préparation à l'envoi du message au serveur ****/
    char message[NB_CARACTERES];
    printf("Veuillez entrer votre message:\n");
    fgets(message, NB_CARACTERES, stdin);
    char *findReturn = strchr(message,'\n'); // Renvoie null si pas trouvé
    if(findReturn != NULL) {
      *findReturn = '\0';
    }

    /**** Envoi des messages au serveur ****/
    if(send(descripteurServeur, message, strlen(message)+1, 0) == -1){
      perror("Erreur: Envoie du message au serveur");
      exit(1);
    }
    printf("Message Envoyé au serveur\n");
  
    /**** Reception du message du serveur ****/
    int r;
    printf("En attente de réception d'un message...\n");
    int nbOctetsLu = recv(descripteurServeur, &r, sizeof(r), 0);
    if(nbOctetsLu == -1) {
      perror("Erreur: réception du message");
      exit(1);
    } else if(nbOctetsLu == 0) {
      printf("Erreur: La socket a été fermée");
    }
    printf("Réponse reçue : %d\n", r);
  }

  /**** Fermeture de la socket ****/
  shutdown(descripteurServeur,2);

  printf("Fin du programme\n");
}