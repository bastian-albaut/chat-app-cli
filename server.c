#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

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
	ad.sin_family = AF_INET;
	ad.sin_addr.s_addr = INADDR_ANY; // Attache la socket à toutes les interfaces réseaux locales
	ad.sin_port = htons(atoi(argv[1]));
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


  /**** Accepter une connexion d'un client ****/
  struct sockaddr_in aC ;
  socklen_t lg = sizeof(struct sockaddr_in) ;
  int descripteurClient = accept(descripteurServeur, (struct sockaddr*) &aC,&lg) ;
  if(descripteurClient == -1) {
    perror("Erreur: Acceptation de la connexion du client");
  }
  printf("Client Connecté\n");


  int nbOctetsLu = 1;
  while(nbOctetsLu != 0) {

    /**** Réception du message du client ****/
    char msg[20];
    int nbOctetsLu = recv(descripteurClient, msg, sizeof(msg), 0);
    if(nbOctetsLu == -1) {
      perror("Erreur: réception du message");
      exit(1);
    } else if(nbOctetsLu == 0) {
      printf("La socket a été fermée par le client\n");
      break;
    }
    printf("Message reçu : %s\n", msg) ;
    

    /**** Envoi du message au client ****/
    int r = 10 ;
    if(send(descripteurClient, &r, sizeof(int), 0) == -1) {
      perror("Erreur: Envoi du message");
      exit(1);
    }
    printf("Message Envoyé\n");
  }

  /**** Fermeture des sockets ****/
  shutdown(descripteurClient, 2); 
  shutdown(descripteurServeur, 2);

  printf("Fin du programme\n");
}