#include "../../../include/server/file/upload_file_server.h"
#include "../../../include/constants.h"
#include "../../../include/server/global_server.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int is_send_file_message(char* message) {
  if (strncmp(message, "/sendfile", 5) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_send_file_message(char* message, int socketClient) {

  // Message does not follow the good format
  if(!is_good_format_send_file_message(message)) {
    char* message = "Send file message have to follow the format /sendfile <file_name> <file_size>";
    send_response(socketClient, SEND_FILE_ERROR, message, NULL);
    return;
  }

  // Get the name and the size of the file
  char* file_name;
  int sizeFile;
  get_file_name_and_size(message, &file_name, &sizeFile);

  // Connect to the new socket create by the client
  int socketFile;
  init_socket(&socketFile, 0, 1);
  connection_request(&socketFile, IP_LOCAL, PORT_SEND_FILE_SOCKET, 1);

  // Create thread to receive the content of the file
  ThreadArgsRecvFile* args = malloc(sizeof(ThreadArgsRecvFile));
  args->fileName = file_name;
  args->socketFile = socketFile;
  args->sizeFile = sizeFile;

  pthread_t threadId;
  pthread_create(&threadId, NULL, thread_receive_file, (void*) args);
}


int is_file_name_and_size_ok(char* fileName, int size, char* message) {
  if(size == 0) {
    strcpy(message, "The file is empty");
    return 0;
  }

  if(size > MAX_SIZE_FILE) {
    strcpy(message, "The file is too big (> 10Mo)");
    message = "The file is too big (> 10Mo)";
    return 0;
  }

  // Check if another file with the same name already exist
  char* filePath = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(filePath, FILE_DIRECTORY_SERVER);
  strcat(filePath, fileName);
  if(access(filePath, F_OK) != -1) {
    strcpy(message, "A file with the same name already exist");
    return 0;
  }

  return 1;
}


int is_good_format_send_file_message(char* message) {
  // Check if the string starts with "/sendfile "
  if (strncmp(message, "/sendfile ", 10) != 0) {
    return 0;
  }

  // Find the end of the file name (the first space after "/sendfile ")
  const char* file_name_end = strchr(message + 10, ' ');
  if (file_name_end == NULL) {
    return 0;
  }

  // Check if there is at least one character after the user name
  if (strlen(file_name_end + 1) == 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


void get_file_name_and_size(char* message, char** file_name, int* file_size) {
  // Find the end of the file name (the first space after "/sendfile ")
  const char* file_name_end = strchr(message + 10, ' ');

  // Get the file name
  int file_name_length = file_name_end - (message + 10);
  *file_name = malloc(file_name_length + 1);
  strncpy(*file_name, message + 10, file_name_length);
  (*file_name)[file_name_length] = '\0';

  // Get the file size
  *file_size = atoi(file_name_end + 1);

  printf("file transfer request receive: %s (%d bytes)\n", *file_name, *file_size);
}


void* thread_receive_file(void* args) {
  ThreadArgsRecvFile* data = (ThreadArgsRecvFile*) args;
  int socketFile = data->socketFile;
  int sizeFile = data->sizeFile;
  char* fileName = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(fileName, data->fileName);

  free(data);

  // Check if the file name and the size of the file are correct
  char* messageError = malloc(NB_CHARACTERS * sizeof(char));
  if(!is_file_name_and_size_ok(fileName, sizeFile, messageError)) {
    send_response(socketFile, SEND_FILE_ERROR, messageError, NULL);
    pthread_exit(0);
  }

  // Send client information that the server is ready to receive the file
  send_response(socketFile, REQUEST_SEND_FILE_ACCEPTED, "The server is ready to handle file content", NULL);

  // Create the file
  FILE* file = create_file(fileName, FILE_DIRECTORY_SERVER);

  printf("Waiting for receiving file content...\n");

  // Receive the content of the file
  char buffer[1025];  // Increase buffer size by 1 for null termination
  while (sizeFile > 0) {
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
    sizeFile -= bytesRead;
  }
  printf("File content received\n");

  // Send confirmation to the client
  char* responseMessage = malloc(NB_CHARACTERS * sizeof(char));
  sprintf(responseMessage, "File transfer (%s) success", fileName);
  send_response(socketFile, FILE_TRANSFER_SUCCESS, responseMessage, NULL);

  // Close the socket and the file
  close_socket(socketFile);
  fclose(file);

  pthread_exit(0);
}