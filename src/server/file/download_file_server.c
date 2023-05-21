#include "../../../include/server/file/download_file_server.h"
#include "../../../include/server/global_server.h"
#include "../../../include/constants.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>


int is_recv_file_message(char* message) {
  if (strncmp(message, "/recvfile", 9) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_recv_file_message(char* message, int socketClient) {

  // Message does not follow the good format
  if(!is_good_format_recv_file_message(message)) {
    char* message = "Recv file message have to follow the format /recvfile <file_name>";
    send_response(socketClient, RECV_FILE_ERROR, message, NULL);
    return;
  }

  // Get the name of the file
  char* fileName = get_file_name_recv_file_message(message);

  // Check if the file exist
  char* filePath = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(filePath, FILE_DIRECTORY_SERVER);
  strcat(filePath, fileName);
  if(access(filePath, F_OK) == -1) {
    char* message = "The file does not exist";
    send_response(socketClient, RECV_FILE_ERROR, message, NULL);
    return;
  }

  // Create thread to send the name, the size and the content of the file
  ThreadArgsSendFile* args = malloc(sizeof(ThreadArgsSendFile));
  args->socket = socketClient;
  args->fileName = fileName;

  pthread_t threadId;
  pthread_create(&threadId, NULL, thread_send_file, (void*) args);
}


int is_good_format_recv_file_message(char* message) {
  // Check if the string starts with "/recvfile "
  if (strncmp(message, "/recvfile ", 10) != 0) {
    return 0;
  }

  // Check if there is at least one character after "/recvfile "
  if (strlen(message + 10) == 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


char* get_file_name_recv_file_message(char* message) {
  char* token = strtok(message, " ");
  token = strtok(NULL, " ");
  return token;
}


void* thread_send_file(void* args) {
  ThreadArgsSendFile* data = (ThreadArgsSendFile*) args;
  char* fileName = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(fileName, data->fileName);
  int socketClient = data->socket;

  free(data);

  // Create a new socket to send the file to the client
  int socketFile;
  init_socket(&socketFile, 1, 1);
  name_socket(&socketFile, PORT_RECV_FILE_SOCKET, 1);
  listen_socket(&socketFile, 1, 1);

  // Send confirmation to the client
  char* responseMessage = "The server is ready to send the file";
  send_response(socketClient, SERVER_READY_FILE, responseMessage, NULL);

  // Accept the connection request
  int socketClientFile = accept(socketFile, NULL, NULL);
  printf("Connection accepted...\n");

  // Get the name and the size of the file
  int sizeFile = get_size_recv_file(fileName);
  
  printf(" => Sending file informations (%s, %d bytes)\n", fileName, sizeFile);

  // Send the name and the size of the file
  char* fileNameAndSize = malloc(NB_CHARACTERS * sizeof(char));
  sprintf(fileNameAndSize, "%s %d", fileName, sizeFile);
  send_response(socketClientFile, FILE_TRANSFER_SUCCESS, fileNameAndSize, NULL);

  printf(" => Sending file content\n");

  // Get the FILE* of the file
  char* filePath = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(filePath, FILE_DIRECTORY_SERVER);
  strcat(filePath, fileName);
  FILE* file = fopen(filePath, "r");

  // Send the content of the file
  char buffer[1024];
  int nbBytesRead;
  do {
    nbBytesRead = fread(buffer, sizeof(char), sizeof(buffer) - 1, file);
    // Error reading file
    if (nbBytesRead < 0) {
      perror("Error reading file");
      exit(1);
    }
    // End of file
    if(nbBytesRead == 0) {
      break;
    }
    if(send(socketClientFile, buffer, nbBytesRead, 0) == -1) {
      perror("Error sending file");
      exit(1);
    }
  } while (nbBytesRead > 0);

  printf("File send\n\n");

  // Close the sockets, the file and free the memory
  close_socket(socketFile);
  close_socket(socketClientFile);
  fclose(file);
  free(fileName);

  pthread_exit(0);
}


int get_size_recv_file(char* fileName) {
  char* filePath = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(filePath, FILE_DIRECTORY_SERVER);
  strcat(filePath, fileName);
  FILE* file = fopen(filePath, "r");

  if(file == NULL) {
    perror("Error: Opening file");
    exit(1);
  }

  // Get the size of the file
  fseek(file, 0, SEEK_END);
  int fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  return fileSize;
}