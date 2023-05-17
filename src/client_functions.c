#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include "../include/global.h"
#include "../include/client_functions.h"
#include "../include/constants.h"

/* Global variables */

// Socket of the server get from the client
int socketServerFromClient;

// Semaphore for the capacity of the server
int idSemaphore;


/**
 * Function thread for sending messages to the server
 *
 * @param socket The socket of the server
 *
 * @return void
 */
void* thread_send(void *socket) {
  int *socketServer = (int*) socket;

  // Loop to send message(s) to the server
  while(1) {

    // Preparing to send the message to the server
    char message[NB_CHARACTERS];
    get_input(message, NB_CHARACTERS, NULL);

    handle_message(message, *socketServer);
  }
}


/**
 *
 * Initialize the socket of the client
 *
 * @return void
 */
void init_socket_client() {

  socketServerFromClient = socket(PF_INET, SOCK_STREAM, 0);

  if(socketServerFromClient == -1) {
    perror("Error: Creation of socket");
    exit(1);
  }
}


/**
 * Send a connection request to the server
 *
 * @param ipAdress The ip adress of the server
 * @param port The port of the server
 *
 * @return void
 */
void connection_request(char* ipAdress, char* port) {
  struct sockaddr_in adress;

  adress.sin_family = AF_INET;
  inet_pton(AF_INET, ipAdress, &(adress.sin_addr));
  adress.sin_port = htons(atoi(port));
  
  socklen_t sizeAdress = sizeof(adress);
  
  if(connect(socketServerFromClient, (struct sockaddr *) &adress, sizeAdress) == -1){
    perror("Error: Server connection request");
    exit(1);
  }

  printf(" __        __   _                            \n");
  printf(" \\ \\      / /__| | ___ ___  _ __ ___   ___  \n");
  printf("  \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ \n");
  printf("   \\ V  V /  __/ | (_| (_) | | | | | |  __/ \n");
  printf("    \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___| \n");
  printf("                                                    \n");
  printf(BOLD "            Welcome to the server!               \n\n\n" RESET);

  printf(GREY "Type /help to see the list of commands\n\n\n" RESET);
}


/**
 * Choose and send pseudo to the server
 *
 * @return void
 */
void send_pseudo() {

  int pseudoIsOk = 0;

  while(!pseudoIsOk) {
    // Send the pseudo to the server
    char pseudo[NB_CHARACTERS];

    get_input(pseudo, NB_CHARACTERS, "Please enter your pseudo:");

    send_message(socketServerFromClient, pseudo, NULL);

    // Receive the response from the server 
    Response* response = malloc(sizeof(Response));
    int nbByteRead = recv_response(socketServerFromClient, response);

    if(nbByteRead == 0 || nbByteRead == -1) {
      printf("The connection was cut on the server side\n");
      exit(1);
    }

    if(response->code == PSEUDO_ACCEPTED) {
      pseudoIsOk = 1;
    }
    print_response(response);
  }
}


/**
 * Function to handle the SIGINT signal (CTRL + C)
 *
 * @param signal The signal to handle
 *
 * @return void
 */
void interrupt_handler(int signal) {
  printf("\n");
  close_socket(socketServerFromClient);
  printf("  =>  Server socket is closed\n");
  printf("\n========== END OF CLIENT ==========\n");
  exit(0);
}


/**
 * Get the input of the user
 *
 * @param message The message to store the input
 * @param size The size of the message
 * @param prompt The prompt to display before getting the input
 *
 * @return void
 */
void get_input(char* message, int size, char* prompt) {
  if(prompt != NULL) {
    printf("%s\n", prompt);
  }
  
  fgets(message, size, stdin);
  char *findReturn = strchr(message,'\n'); // Return null if not found
  if(findReturn != NULL) {
    *findReturn = '\0';
  }
}


/**
 * Send a message to the server with the socket specified in parameter
 *
 * @param socketServer The socket of the server to send the message
 * @param message The message to send
 * @param prompt The prompt to display after sending the message
 *
 * @return void
 */
void send_message(int socketServer, char* message, char* prompt) {
  if(send(socketServer, message, strlen(message)+1, 0) == -1) {
    perror("Error: Send message");
    exit(1);
  }
  if(prompt != NULL) {
    printf("%s\n", prompt);
  }
}


/**
 * Recv an informative, a success, a redirection or an error response of the server with the socket specified in parameter
 *
 * @param socketServer The socket of the server to recv the message
 * @param response The response to store the message
 *
 * @return The number of bytes received
 */
int recv_response(int socketServer, Response* response) {

  char buffer[RESPONSE_BUFFER_SIZE];
  int nbBytes = recv(socketServer, buffer, sizeof(buffer), 0);
  if(nbBytes == -1) {
    perror("Error: Receiving the response");
    exit(1);
  }

  if(nbBytes == 0) {
    return 0;
  }

  // Deserialize the response
  deserialize_response(buffer, sizeof(buffer), response);

  return nbBytes;
}


/**
 * Deserialize a response from the buffer specified in parameter and store it in an instance of Response
 *
 * @param buffer The buffer to deserialize
 * @param sizeBuffer The length of the buffer
 * @param response The response to store the deserialized buffer
 *
 * @return void
 */
void deserialize_response(char* buffer, size_t sizeBuffer, Response* response) {
  
  // Unpack the code field from the buffer
  int code;
  memcpy(&code, buffer, sizeof(code));
  response->code = ntohl(code);
  buffer += sizeof(code);
  sizeBuffer -= sizeof(code);

  // Allocate memory for the message field and copy the serialized message into it
  size_t lengthMessage = strlen(buffer) + 1; 
  response->message = malloc(lengthMessage);
  if (response->message == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }
  memcpy(response->message, buffer, lengthMessage);
  buffer += lengthMessage;
  sizeBuffer -= lengthMessage;

  // Allocate memory for the from field and copy the serialized from field into it
  size_t lengthFrom = strlen(buffer) + 1;
  response->from = malloc(lengthFrom);
  if (response->from == NULL) {
    printf("Memory allocation failed\n");
    exit(1);
  }
  memcpy(response->from, buffer, lengthFrom);
}


/**
 * Print the response specified in parameter with a color depending on the code of the response
 *
 * @param response The response to print
 *
 * @return void
 */
void print_response(Response* response) {
  if(response->code >= 100 && response->code < 200) {
    printf(BLUE "%d - %s" RESET, response->code, response->message);
  } 
  else if(response->code >= 200 && response->code < 300) {
    if(response->code == HELP_SUCCESS) {
      printf(GREEN "%s" RESET, response->message);
    } 
    else if(response->code == LIST_FILE_SUCCESS) {
      printf(GREEN "\n------ List of server file(s) ------\n");
      printf("%s", response->message);
      printf("------ End list ------" RESET);
      printf("\n\n");
    }
    else {
      printf(GREEN "%d - %s" RESET, response->code, response->message);
    }
  } 
  else if(response->code >= 300 && response->code < 400) {
    if(response->code == MESSAGE_GLOBAL_REDIRECT) {
      printf(YELLOW "%s: %s" RESET, response->from, response->message);
    } 
    else if(response->code == MESSAGE_PRIVATE_REDIRECT) {
      printf(YELLOW "%s (Message Privé): %s" RESET, response->from, response->message);
    }
    else {
      printf(YELLOW "%d - %s" RESET, response->code, response->message);
    }
  } 
  else {
    printf(RED "%d - %s" RESET , response->code, response->message);
  }
  printf("\n");
}


/**
 * Initialize the semaphore for handle capacity of clients
 *
 * @return void
 */
void init_semaphore_client() {
  int keySemaphore = ftok("command.txt", 'r');
  if (keySemaphore == -1) {
    perror("Error ftok");
    exit(1);
  }

  int nbSemaphores = 1;  
  idSemaphore = semget(keySemaphore, nbSemaphores, 0666);
  if(idSemaphore == -1){
    perror("Error semget");
    exit(1);
  }
}



/**
 * Decrement the semaphore in global variables to take place in it
 *
 * @return void
 */
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


/**
 * Handle the message specified in parameter
 *
 * @param message The message to handle
 * @param socketServer The socket of the server
 *
 * @return void
 */
void handle_message(char* message, int socketServer) {
  if(is_list_file_message(message)) {
    handle_list_file_message(message);
    return;
  }

  if(is_send_file_message(message)) {
    handle_send_file_message(message, socketServer);
    return;
  }

  // Sending the message to the server
  send_message(socketServer, message, NULL);
}


/**
 * Detect if the message corresponding to list file command (start with "/listfiles")
 *
 * @param message The message to check
 *
 * @return 1 if the message is a list file command | 0 if the message is not a list file command
 */
int is_list_file_message(char* message) {
  if(strncmp(message, "/listfiles local", 16) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Display the list of files of the directory FILE_DIRECTORY_CLIENT
 *
 * @return void
 */
void handle_list_file_message(char* message) {

  if(!is_good_format_list_file_message(message)) {
    printf("Error: Bad format for the message\n");
    return;
  }

  DIR *directory;
  struct dirent *file;
  directory = opendir(FILE_DIRECTORY_CLIENT);

  if(directory == NULL) {
    perror("Error: Opening directory");
    exit(1);
  }

  printf("\n------ List of local file(s) ------\n");
  while((file = readdir(directory)) != NULL) {
    if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
      printf("%s\n", file->d_name);
    }
  }
  printf("------ End list ------\n\n");
  closedir(directory);
}



/**
 * Detect if the message corresponding to the format /listfiles local
 *
 * @param param description
 *
 * @return description
 */
int is_good_format_list_file_message(char* message) {
  
  char* token = strtok(message, " ");
  if(token == NULL) {
    return 0;
  }

  if(strcmp(token, "/listfiles") != 0) {
    return 0;
  }

  token = strtok(NULL, " ");
  if(token == NULL) {
    return 0;
  }

  if(strcmp(token, "local") != 0) {
    return 0;
  }

  return 1;
}


/**
 * Detect if the message corresponding to send file command (start with "/sendfile")
 *
 * @param message The message to check
 *
 * @return 1 if the message is a send file command | 0 if the message is not a send file command
 */
int is_send_file_message(char* message) {
  if(strncmp(message, "/sendfile", 9) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Handle the send file command
 *
 * @param message The message to handle
 * @param socketServer The socket of the server
 *
 * @return void
 */
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


/**
 * Detect if the message corresponding to the format /sendfile <file_name>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_send_file_message(char* message) {
  // Check if the string starts with "/sendfile "
  if(strncmp(message, "/sendfile ", 10) != 0) {
    return 0;
  }

  // Chech if there is at least one character after "/sendfile "
  if(strlen(message + 10) == 0) {
    return 0;
  }
}


/**
 * Get the file name in the message
 *
 * @param message The message to check
 *
 * @return The file name in the message
 */
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


/**
 * Detect if the file specified in parameter exist
 *
 * @param fileName The name of the file to check
 *
 * @return 1 if the file exist | 0 if the file doesn't exist
 */
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


/**
 * Send name and size of the file specified in parameter to the server 
 * Create a new socket to handle the file transfer
 * Send the content of the file by blocks of 1024 bytes with the new socket
 *
 * @param socketServer The socket of the server
 * @param fileName The name of the file to send
 *
 * @return void
 */
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


/**
 * Function thread for handle file transfer
 *
 * @param socket The socket of the server
 *
 * @return void
 */
void* thread_file_transfer(void *arg) {
  FILE* file = (FILE*) arg;

  // Configure the new socket to handle file transfer
  int socketFile = init_socket_file();
  name_socket_file(socketFile, PORT_FILE_SOCKET);
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
        if(nbByteRead == 0) {
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



/**
 *
 * Initialize the socket in TCP which will be used to communicate with server to handle file transfer
 *
 * @return The socket of the file
 */
int init_socket_file() {
	int socketFile = socket(PF_INET, SOCK_STREAM, 0);
	
  if(socketFile == -1) {
    perror("Error: Creation of socket");
    exit(1);
  }

  // Allow to use address again
  int optval = 1;
  setsockopt(socketFile, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  printf("Socket for file created\n");
  return socketFile;
}

/**
 * Name the socket of the file transfer
 *
 * @param socket The socket of the file transfer
 * @param port The port of the socket
 *
 * @return void
 */
void name_socket_file(int socketFile, int port) {
  struct sockaddr_in adress;
  socklen_t sizeAdress= sizeof(adress);

  adress.sin_family = AF_INET; 
  adress.sin_addr.s_addr = INADDR_ANY;
  adress.sin_port = htons(port);

  if(bind(socketFile, (struct sockaddr*)&adress, sizeAdress) == -1) {
    perror("Error: Socket naming");
    exit(1);
  }
  printf(" => Named Socket file\n");
}


/**
 * Listen the socket for file transfer
 *
 * @return void
 */
void listen_socket_file(int socketFile) {
  if(listen(socketFile, 2) == -1) {
    perror("Error: Socket listening");
    exit(1);
  }
  printf(" => Socket file listening\n");
}