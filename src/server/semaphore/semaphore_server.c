#include "../../../include/server/semaphore/semaphore_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int idSemaphore;

void init_semaphore_server() {
  // Generate a key for the semaphore
  int keySemaphore = ftok("command.txt", 'r');
  if (keySemaphore == -1) {
    perror("Error ftok");
    exit(1);
  }

  // Create one semaphore with the key
  int nbSemaphores = 1;  
  idSemaphore = semget(keySemaphore, nbSemaphores, IPC_CREAT | 0666);
  if(idSemaphore == -1){
    perror("Error semget");
    exit(1);
  }
}


void set_capacity_semaphore(int capacity) {
  // Initialize semun structure
  union semun startnbPlacesSemaphore;
  startnbPlacesSemaphore.val = capacity;

  // Set the capacity of the semaphore
  if (semctl(idSemaphore, 0, SETVAL, startnbPlacesSemaphore) == -1){
    perror("Error semctl");
    exit(1);
  }
  printf("Capacity: %d client(s)\n\n", startnbPlacesSemaphore.val);
}


void deletion_semaphore() {
  // Check if the semaphore exists
  if (idSemaphore == 0) {
    return;
  }

  if (semctl(idSemaphore, 0, IPC_RMID) == -1) {
    perror("Error semctl");
    exit(1);
  }
  printf("Semaphore deleted\n");
}


void leave_place_semaphore() {
  // Increment the semaphore to leave place in it
  struct sembuf operation = {0, 1, 0}; 

  if(semop(idSemaphore, &operation, 1) == -1) {
    perror("Error: Leaving place in the semaphore");
    exit(1);
  }

  display_remaining_places_semaphore();
}


void display_remaining_places_semaphore() {
  // Get the number of places in the semaphore
  int countPlaces = semctl(idSemaphore, 0, GETVAL);

  if(countPlaces == -1){
    perror("Error: Getting semaphore nbPlaces");
    exit(1);
  }
  printf("Remaining places: %d\n\n", countPlaces);
}