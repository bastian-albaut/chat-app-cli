#include "../../../include/client/semaphore/semaphore_client.h"
#include "../../../include/client/global_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>


void init_semaphore_client() {
  // Generate the key for the semaphore 
  int keySemaphore = ftok("./assets/command.txt", 'r');
  if (keySemaphore == -1) {
    perror("Error ftok");
    exit(1);
  }

  // Get the id of the semaphore
  int nbSemaphores = 1;  
  idSemaphore = semget(keySemaphore, nbSemaphores, 0666);
  if(idSemaphore == -1){
    perror("Error semget");
    exit(1);
  }
}


void take_place_semaphore() {
  
  struct sembuf operation = {0, -1, IPC_NOWAIT}; // Decrement the semaphore (non-blocking)
  
  if (semop(idSemaphore, &operation, 1) == -1) {
    if (errno == EAGAIN) {
      // No place available in the semaphore
      printf("\n\n\nNo place available in the semaphore. Please try again later.\n");
      exit(0);
    }
    perror("Error: Taking place in the semaphore");
    exit(1);
  }
}