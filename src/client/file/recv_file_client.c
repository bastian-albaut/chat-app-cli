#include "../../../include/client/file/recv_file_client.h"
#include "../../../include/client/global_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int is_recv_file_message(char* message) {
  if (strncmp(message, "/recvfile", 9) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_recv_file_message(char* message, int socketServer) {
  if(!is_good_format_recv_file_message(message)) {
    printf("Error: Bad format of the message\n");
    return;
  }

  // Send the command to the server
  send_message(socketServer, message, NULL);

  printf("Waiting for the server to create the socket...\n");

  // Receive the confirmation of the server
  Response* response = malloc(sizeof(Response));
  int nbByteRead = recv_response(socketServer, response);

  print_response(response);

  // Create new thread to handle the file transfer
  pthread_t thread;
  pthread_create(&thread, NULL, thread_recv_file, (void*)NULL);
}


int is_good_format_recv_file_message(char* message) {
  // Check if the string starts with "/recvfile "
  if(strncmp(message, "/recvfile ", 10) != 0) {
    return 0;
  }

  // Chech if there is at least one character after "/recvfile "
  if(strlen(message + 10) == 0) {
    return 0;
  }

  return 1;
}


void* thread_recv_file(void* args) {

  // Connect to the new socket create by the server
  int socketFile;
  init_socket(&socketFile, 0, 1);
  connection_request_file_transfer(socketFile, PORT_RECV_FILE_SOCKET);

  // Receive the file name and size
  Response* response = malloc(sizeof(Response));
  int nbByteRead = recv_response(socketFile, response);
  char* fileName = strtok(response->message, " ");
  char* fileSizeString = strtok(NULL, " ");
  int fileSize = atoi(fileSizeString);

  printf("Receiving file %s (%d bytes)...\n", fileName, fileSize);

  // Create the file
  FILE* file = create_file(fileName, FILE_DIRECTORY_CLIENT);

  // Receive the content of the file
  char buffer[1025];  // Increase buffer size by 1 for null termination
  while (fileSize > 0) {
    int bytesRead = recv(socketFile, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
      perror("Error receiving file");
      pthread_exit(0);
    } else if (bytesRead == 0) {
      printf("Connection closed by client\n");
      pthread_exit(0);
      break;
    }
    printf(" => %d bytes received\n", bytesRead);

    buffer[bytesRead] = '\0';  // Add null termination
    if (fputs(buffer, file) == EOF) {
      perror("Error writing to file");
      pthread_exit(0);
    }
    fileSize -= bytesRead;
  }
  printf("File content received\n");

  // Close the socket and the file
  close_socket(socketFile);
  fclose(file);

  pthread_exit(0);
}


void connection_request_file_transfer(int socketFile, int port) {
  struct sockaddr_in adress;
  char* ipAdress = malloc(16);
  strcpy(ipAdress, "127.0.0.1");

  adress.sin_family = AF_INET;
  inet_pton(AF_INET, ipAdress, &(adress.sin_addr));
  adress.sin_port = htons(port);
  
  socklen_t sizeAdress = sizeof(adress);
  
  if(connect(socketFile, (struct sockaddr *) &adress, sizeAdress) == -1){
    perror("Error: Server connection request");
    exit(1);
  }

  printf(" => Connected to the server\n");
}