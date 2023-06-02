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

  // Send socket server and message to the thread
  ThreadArgsRecvFile* args = malloc(sizeof(ThreadArgsRecvFile));
  args->socketServer = socketServer;
  args->message = message;

  // Create new thread to handle the file transfer
  pthread_t thread;
  pthread_create(&thread, NULL, thread_recv_file, (void*)args);
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
  ThreadArgsRecvFile* data = (ThreadArgsRecvFile*) args;
  int socketServer = data->socketServer;
  char* message = data->message;

  free(data);

  // Create a new socket to receive the file from the server
  int socketFile;
  init_socket(&socketFile, 1, 1);
  name_socket(&socketFile, PORT_RECV_FILE_SOCKET, 1);
  listen_socket(&socketFile, 1, 1);

  // Send the command to the server (with the old socket)
  send_message(socketServer, message, NULL);

  // Accept the connection with the new socket
  int socketServerFile = accept(socketFile, NULL, NULL);
  printf("Connection accepted...\n");

  // Receive the file name and size from the server with the new socket
  Response* response = malloc(sizeof(Response));
  int nbByteRead = recv_response(socketServerFile, response);
  char* fileName = strtok(response->message, " ");
  char* fileSizeString = strtok(NULL, " ");
  int fileSize = atoi(fileSizeString);

  // Create the file
  FILE* file = create_file(fileName, FILE_DIRECTORY_CLIENT);

  // Receive the content of the file
  char buffer[1025];  // Increase buffer size by 1 for null termination
  while (fileSize > 0) {
    int bytesRead = recv(socketServerFile, buffer, sizeof(buffer) - 1, 0);
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
  printf("File received\n\n");
  
  // Close the sockets and the file
  close_socket(socketFile);
  close_socket(socketServerFile);
  fclose(file);

  pthread_exit(0);
}