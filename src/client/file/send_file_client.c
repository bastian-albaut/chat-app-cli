#include "../../../include/client/file/send_file_client.h"
#include "../../../include/client/global_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>


int is_send_file_message(char* message) {
  if(strncmp(message, "/sendfile", 9) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_send_file_message(char* message, int socketServer) {
  if(!is_good_format_send_file_message(message)) {
    printf("Error: Bad format of the message\n");
    return;
  }

  char* fileName = get_file_name(message);
  if(fileName == NULL) {
    printf("Error: No file name specified\n");
    return;
  }

  if(!is_file_exist(fileName)) {
    printf("Error: File %s doesn't exist\n", fileName);
    return;
  }

  // Sending the file to the server
  send_file(socketServer, fileName);
}


int is_good_format_send_file_message(char* message) {
  // Check if the string starts with "/sendfile "
  if(strncmp(message, "/sendfile ", 10) != 0) {
    return 0;
  }

  // Chech if there is at least one character after "/sendfile "
  if(strlen(message + 10) == 0) {
    return 0;
  }

  return 1;
}


char* get_file_name(char* message) {
  char* content = malloc(NB_CHARACTERS * sizeof(char));
  int i = 0;
  int j = 0;
  while(message[j] != ' ') {
    j++;
  }
  j++;
  while(message[j] != '\0') {
    content[i] = message[j];
    i++;
    j++;
  }
  content[i] = '\0';

  return content;
}


int is_file_exist(char* fileName) {
  DIR *directory;
  struct dirent *file;
  directory = opendir(FILE_DIRECTORY_CLIENT);

  if(directory == NULL) {
    perror("Error: Opening directory");
    exit(1);
  }

  while((file = readdir(directory)) != NULL) {
    if(strcmp(file->d_name, fileName) == 0) {
      return 1;
    }
  }

  return 0;
}


void send_file(int socketServer, char* fileName) {

  // Concatenate the directory and the file name
  char* filePath = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(filePath, FILE_DIRECTORY_CLIENT);
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
  char* fileSizeString = malloc(NB_CHARACTERS * sizeof(char));
  sprintf(fileSizeString, "%d", fileSize);

  printf("Sending file %s (%d bytes)...\n", fileName, fileSize);

  // Create new thread to handle the file transfer
  pthread_t thread;
  pthread_create(&thread, NULL, thread_file_transfer, (void*)file);

  // Send the command the name and the size of the file whith the format "/sendfile <file_name> <file_size>"
  char* command = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(command, "/sendfile ");
  strcat(command, fileName);
  strcat(command, " ");
  strcat(command, fileSizeString); 
  if(send(socketServer, command, strlen(command), 0) == -1) {
    perror("Error sending command of sending file");
    exit(1);
  }
}


void* thread_file_transfer(void *arg) {
  FILE* file = (FILE*) arg;

  // Configure the new socket to handle file transfer
  int socketFile;
  init_socket(&socketFile, 1, 1);
  name_socket(&socketFile, PORT_SEND_FILE_SOCKET, 1);
  listen_socket_file(socketFile);

  // Accept the connection of the server
  int socketFileServer = accept(socketFile, NULL, NULL);

  printf("Server connected to handle file transfer\n");

  // Receive the confirmation of the server
  Response* response = malloc(sizeof(Response));
  int nbByteRead = recv_response(socketFileServer, response);

  if(nbByteRead == 0) {
    printf("The connection was cut on the server side\n");
  } else if(nbByteRead == -1) {
    perror("Error: Receiving the response");
  } else {
    
    // If the server is ready to receive the file
    if(response->code == REQUEST_SEND_FILE_ACCEPTED) {

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
        if (send(socketFileServer, buffer, nbBytesRead, 0) == -1) {
          perror("Error sending file");
          exit(1);
        }
      } while (nbBytesRead > 0);

      // Wait for the confirmation of the server
      Response* response = malloc(sizeof(Response));
      int nbByteRead = recv_response(socketFileServer, response);
      if(nbByteRead > 0) {
        print_response(response);
      }
    } else {

      // Display the server error
      print_response(response);
    }
  }

  // Close the sockets, the file and free the memory
  close_socket(socketFileServer);
  close_socket(socketFile);
  fclose(file);
  free(response);

  pthread_exit(0);
}


void listen_socket_file(int socketFile) {
  if(listen(socketFile, 2) == -1) {
    perror("Error: Socket listening");
    exit(1);
  }
  printf(" => Socket file listening\n");
}