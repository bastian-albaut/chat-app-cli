#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <dirent.h>
#include "../include/server_functions.h"
#include "../include/list.h"
#include "../include/constants.h"
#include "../include/global.h"


/* Global variables */

// Semaphore to handle capacity of clients
int idSemaphore;


/**
 * Function thread to manage each client
 *
 * @param args An instance of ThreadArgs structure with socketClient and listClient
 *
 * @return void
 */
void* thread_client(void* args) {
  ThreadArgs* data = (ThreadArgs*) args;
  int socketClient = data->socketClient;
  pthread_t* ptrTempIdThread = data->ptrTempIdThread;

  // Get pseudo of the client 
  char* pseudo = malloc(NB_CHARACTERS_PSEUDO_MAX * sizeof(char));
  if(handle_pseudo_client(socketClient, pseudo) == -1) {
    remove_client(socketClient, *ptrTempIdThread);
    pthread_exit(0);
  }

  // Add the client to the list
  Node* currentClient = insert_first(&listClient, socketClient, pseudo, *ptrTempIdThread);

  // Inform the server that a new client is connected
  printf("Client %s connected !\n", pseudo);
  display_list(&listClient);

  // Loop to receive message and handle messages from client
  while(1) {

    // Receive message from client 
    char message[NB_CHARACTERS];
    int nbByteRead = recv_message(socketClient, message);

    // If the client is disconnected
    if(nbByteRead == 0 || nbByteRead == -1) {
      remove_client(socketClient, *ptrTempIdThread);
      break;
    } 
    
    printf("Message receive: %s\n", message);

    handle_message(message, socketClient, pseudo, pthread_self());
  }
}


/**
 *
 * Initialize the socket in TCP which will be used to communicate with clients
 *
 * @return void
 */
void init_socket_server() {
	socketServer = socket(PF_INET, SOCK_STREAM, 0);
	
  if(socketServer == -1) {
    perror("Error: Creation of socket");
    exit(1);
  }

  // Allow to use address again
  int optval = 1;
  setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  
  printf("Socket Created");
}

/**
 * Name the socket of the server
 *
 * @param port The port of the socket
 *
 * @return void
 */
void name_socket_server(char* port) {
  struct sockaddr_in adress;
  socklen_t sizeAdress= sizeof(adress);

  adress.sin_family = AF_INET; 
  adress.sin_addr.s_addr = INADDR_ANY;
  adress.sin_port = htons(atoi(port));

  if(bind(socketServer, (struct sockaddr*)&adress, sizeAdress) == -1) {
    perror("Error: Socket naming");
    exit(1);
  }
  printf(" => Named Socket successfully");
}


/**
 * Listen the socket server
 *
 * @return description
 */
void listen_socket_server() {
  if(listen(socketServer, 10) == -1) {
    perror("Error: Socket listening");
    exit(1);
  }
  printf(" => Socket listening\n\n");
}


/**
 * handle acceptation of client
 *
 * @return The socket client if acceptation work correctly | -1 if not
 */
int accept_client() {
  struct sockaddr_in adressClient;
  socklen_t sizeAdressClient = sizeof(adressClient);

  int socketClient = accept(socketServer, (struct sockaddr*)&adressClient, &sizeAdressClient);

  return socketClient;
}


/**
 * Handle the pseudo of the client
 *
 * @param socketClient The socket of the client which want to get a pseudo
 *
 * @return 1 if handle pseudo work correctly | -1 if not
 */
int handle_pseudo_client(int socketClient, char* pseudo) {
  // Get pseudo of the client 
  char* pseudoGet = get_pseudo(socketClient);
  
  if(pseudoGet == NULL) {
    return -1;
  }

  // Set pseudo of client in the list 
  if(pseudoGet != NULL) {
    strcpy(pseudo, pseudoGet);
    return 1;
  }
}


/**
 * Get pseudo of the client and check if it is already used
 *
 * @param socketClient The socket of the client which want to get a pseudo
 *
 * @return The pseudo of the client
 */
char* get_pseudo(int socketClient) {

  char* pseudo = malloc(NB_CHARACTERS_PSEUDO_MAX * sizeof(char));
  int code;
  int pseudoAlreadyUsed = 1;
  int pseudoGoodFormat = 0;

  while(pseudoAlreadyUsed ) {
    
    int nbByteRead = recv_message(socketClient, pseudo);
    if(nbByteRead == 0) {
      printf("Client disconnected during pseudo step\n");
      return NULL;
    }

    // Check if pseudo is in the good format
    char* responseMessage = malloc(NB_CHARACTERS * sizeof(char));
    if(!check_format_pseudo(pseudo, responseMessage)) {
      send_response(socketClient, PSEUDO_BAD_FORMAT, responseMessage, NULL);
      continue;
    } else {
      pseudoGoodFormat = 1;
    }

    // Check if pseudo is already used
    if(search_element_pseudo(&listClient, pseudo) != NULL) {
      char* message = "This pseudo is already used";
      send_response(socketClient, PSEUDO_ALREADY_USED, message, NULL);
    } else {
      pseudoAlreadyUsed = 0;
    }
  }

  // Send response to client
  char* message = "This pseudo is accepted";
  send_response(socketClient, PSEUDO_ACCEPTED, message, NULL);

  return pseudo;
}


/**
 * Check if the pseudo is in the good format
 *
 * @param pseudo The pseudo to check
 *
 * @return 1 if the pseudo is in the good format | 0 if is not in the good format
 */
int check_format_pseudo(char* pseudo, char* responseMessage) {
  // Check if the pseudo is not too long
  if(strlen(pseudo) > NB_CHARACTERS_PSEUDO_MAX) {
    sprintf(responseMessage, "Your pseudo is too long(>%d characters)", NB_CHARACTERS_PSEUDO_MAX);
    return 0;
  }

  // Check if the pseudo is not too short
  if(strlen(pseudo) < NB_CHARACTERS_PSEUDO_MIN) {
    sprintf(responseMessage, "Your pseudo is too short (<%d characters)", NB_CHARACTERS_PSEUDO_MIN);
    return 0;
  }

  // Check pseudo does not contain space
  if(strchr(pseudo, ' ') != NULL) {
    strcpy(responseMessage, "Your pseudo cannot contain space");
    return 0;
  }

  return 1;
}


/**
 * Handle the message of the client
 *
 * @param message The string to handle
 * @param socketClient The socket of the client who send the message
 * @param pseudo The pseudo of the client who send the message
 *
 * @return void
 */
void handle_message(char* message, int socketClient, char* pseudo, pthread_t threadId) {
  if(is_special_command(message)) {
    if(is_global_message(message)) {
      handle_global_message(message, socketClient, pseudo);
      return;
    }

    if(is_private_message(message)) {
      handle_private_message(message, socketClient, pseudo);
      return;
    }

    if(is_logout_message(message)) {
      handle_logout_message(message, socketClient, threadId);
      return;
    }

    if(is_help_message(message)) {
      handle_help_message(message, socketClient);
      return;
    }

    if(is_send_file_message(message)) {
      handle_send_file_message(message, socketClient);
      return;
    }

    if(is_list_files_server(message)) {
      handle_list_files_server(message, socketClient);
      return;
    }

    if(is_recv_file_message(message)) {
      handle_recv_file_message(message, socketClient);
      return;
    }
  }
  char* responseMessage = "Command not found";
  send_response(socketClient, COMMAND_NOT_FOUND, responseMessage, NULL);
}


/**
 * Detect if the message corresponding to a special command (start with "/")
 *
 * @param param message The string to check
 *
 * @return 1 if the message is a special command | 0 if the message is not a special command
 */
char is_special_command(char* message) {
  if (strncmp(message, "/", 1) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a private message (start with "/mp")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a private message | 0 if the message is not a private message
 */
int is_private_message(char* message) {
  if (strncmp(message, "/mp", 3) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a global message (start with "/all")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a global message | 0 if the message is not a global message
 */
int is_global_message(char* message) {
  if (strncmp(message, "/all", 4) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a logout message (start with "/logout")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a logout message | 0 if the message is not a logout message
 */
int is_logout_message(char* message) {
  if (strncmp(message, "/logout", 7) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a help message (start with "/help")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a help message | 0 if the message is not a help message
 */
int is_help_message(char* message) {
  if (strncmp(message, "/help", 5) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a send file message (start with "/sendfile")
 * 
 * @param message The string to check
 * 
 * @return 1 if the message is a send file message | 0 if the message is not a send file message
 */
int is_send_file_message(char* message) {
  if (strncmp(message, "/sendfile", 5) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a list files server message (start with "/listfiles server")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a list files server message | 0 if the message is not a list files server message
 */
int is_list_files_server(char* message) {
  if (strncmp(message, "/listfiles server", 17) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Detect if the message corresponding to a recv files client message (start with "/recvfile")
 *
 * @param message The string to check
 *
 * @return 1 if the message is a recv files client message | 0 if the message is not a recv files client message
 */
int is_recv_file_message(char* message) {
  if (strncmp(message, "/recvfile", 9) == 0) {
    return 1;
  } else {
    return 0;
  }
}


/**
 * Send private message to a client. OR send an error message if the private message
 * is not in the correct format or if the pseudo of the client does not exist. OR 
 * do nothing if the message is not a private message
 *
 * @param message The string to handle
 * @param socketClient The socket of the client who send the message
 *
 * @return void
 */
void handle_private_message(char* message, int socketClient, char* pseudoTransmitter) {
  int goodFormat = is_good_format_private_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Private message have to follow the format /mp <user> <message>";
    send_response(socketClient, PRIVATE_MESSAGE_BAD_FORMAT, message, NULL);
  } 
  
  // Message follow the good format
  if(goodFormat == 1) {
    // Get pseudo of the private message
    char* pseudo = get_pseudo_private_message(message);

    // Get content of the private message
    char* messagePrivate = get_content_private_message(message);

    // Send private message
    int code = send_private_message(listClient, pseudo, messagePrivate, pseudoTransmitter);

    // Send response to the client who send the private message
    if(code == -1) {
      char* message = "There is no user with this pseudo";
      send_response(socketClient, PRIVATE_MESSAGE_USER_NOT_FOUND, message, NULL);
    } else {
      char* message = "Private message send";
      send_response(socketClient, PRIVATE_MESSAGE_SEND, message, NULL);
    }
  }
}

/**
 * Detect if the message corresponding to the format /mp <user> <message>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_private_message(char* message) {
  // Check if the string starts with "/mp "
  if (strncmp(message, "/mp ", 4) != 0) {
    return 0;
  }

  // Find the end of the user name (the first space after "/mp ")
  const char* user_end = strchr(message + 4, ' ');
  if (user_end == NULL) {
    return 0;
  }

  // Check if there is at least one character after the user name
  if (strlen(user_end + 1) == 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


/**
 * Get the destination pseudo of the private message
 *
 * @param message The string to split to get the pseudo
 *
 * @return The pseudo of the private message
 */
char* get_pseudo_private_message(char* message) {
  char* pseudo = malloc(NB_CHARACTERS_PSEUDO_MAX * sizeof(char));
  int i = 0;
  int j = 4;
  while(message[j] != ' ') {
    pseudo[i] = message[j];
    i++;
    j++;
  }
  pseudo[i] = '\0';

  return pseudo;
}


/**
 * Get content of the private message
 *
 * @param message The string to split to get the content
 *
 * @return The content of the private message
 */
char* get_content_private_message(char* message) {
  char* content = malloc(NB_CHARACTERS * sizeof(char));
  int i = 0;
  int j = 0;
  while(message[j] != ' ') {
    j++;
  }
  j++;
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
 * Send private message to the client with the pseudo specified in parameter
 *
 * @param head The head of the list of clients
 * @param pseudo The pseudo of the client to send the message
 * @param message The message to send
 *
 * @return 1 if the message is send | -1 if the message is not send
 */
int send_private_message(Node* head, char* pseudo, char* message, char* pseudoTransmitter) {
  Node* elementToSend = search_element_pseudo(&head, pseudo);
  if(elementToSend != NULL) {
    send_response(elementToSend->number, MESSAGE_PRIVATE_REDIRECT, message, pseudoTransmitter);
    printf("Message send to the client %s\n", pseudo);
    return 1;
  } else {
    return -1;
  }
}


/**
 * Handle global message. Send the message to all clients.
 *
 * @param head The head of the list of clients
 * @param socketClient The socket of the client who send the message
 * @param message The message to send
 *
 * @return void
 */
void handle_global_message(char* message, int socketClient, char* pseudoTransmitter) {
  int goodFormat = is_good_format_global_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Global message have to follow the format /all <message>";
    send_response(socketClient, GLOBAL_MESSAGE_ERROR, message, NULL);
  } 
  
  // Message follow the good format
  if(goodFormat == 1) {

    // Get content of the global message
    char* globalMessage = get_content_global_message(message);

    // Send message to other clients
    int code = send_to_other_clients(globalMessage, socketClient, pseudoTransmitter);
    if(code == 1) {
      char* message = "Message send to all clients";
      send_response(socketClient, GLOBAL_MESSAGE_SEND, message, NULL);
    } else if(code == -1) {
      char* message = "Error while sending message to all clients";
      send_response(socketClient, GLOBAL_MESSAGE_ERROR, message, NULL);
    } else {
      char* message = "There is no other clients";
      send_response(socketClient, NO_OTHER_USERS, message, NULL);
    }
  }
}


/**
 * Detect if the message corresponding to the format /all <message>
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_global_message(char* message) {
  // Check if the string starts with "/all "
  if (strncmp(message, "/all ", 5) != 0) {
    return 0;
  }

  // Check if there is at least one character after "/all "
  if (strlen(message + 5) == 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


/**
 * Get content of the global message
 *
 * @param message The string to split to get the content
 *
 * @return The content of the global message
 */
char* get_content_global_message(char* message) {
  char* content = malloc(NB_CHARACTERS * sizeof(char));
  int i = 0;
  int j = 5;
  while(message[j] != '\0') {
    content[i] = message[j];
    i++;
    j++;
  }
  content[i] = '\0';

  return content;
}


/**
 * Send message to all clients whithout client corresponding to socketClient
 *
 * @param message The message to send
 * @param socketClient The socket of the client who send the message
 * @param pseudoTransmitter The pseudo of the client who send the message
 *
 * @return 1 if all messages are send | -1 if at least one message is not send | 0 if there is no other clients
 */
int send_to_other_clients(char* message, int socketClient, char* pseudoTransmitter) {
  Node* currentClient = listClient->next;
  int errorCatch = 0;
  int countSend = 0;
  while(currentClient != listClient && errorCatch == 0) {
    if(currentClient->number != socketClient && currentClient->pseudo != NULL) {
      if(send_response(currentClient->number, MESSAGE_GLOBAL_REDIRECT, message, pseudoTransmitter) == -1) {
        errorCatch = 1;
      } else {
        countSend += 1;
      }
    }
    currentClient = currentClient->next;
  }

  // If at least one message is not send
  if(errorCatch == 1) {
    perror("Error: Sending message to other clients");
    return -1;
  } 

  // If there is no other clients
  if(countSend == 0) {
    return 0;
  }

  // If all messages are send
  printf("Message send to other clients\n");
  return 1;

}



/**
 * Handle logout message. Send a message to the client to confirm the logout.
 *
 * @param message The message check
 * @param socketClient The socket of the client who want to logout
 *
 * @return void
 */
void handle_logout_message(char* message, int socketClient, pthread_t threadId) {
  int goodFormat = is_good_format_logout_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Logout message have to follow the format /logout";
    send_response(socketClient, DISCONNECT_BAD_FORMAT, message, NULL);
  }

  // Message follow the good format
  if(goodFormat == 1) {
    remove_client(socketClient, threadId);
  }
}


/**
 * Detect if the message corresponding to the format /logout
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_logout_message(char* message) {
  // Check if the string starts with "/logout"
  if (strncmp(message, "/logout", 7) != 0) {
    return 0;
  }

  // Check if there is no character after "/logout"
  if(strlen(message + 7) != 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


/**
 * Handle help message. Send a message to the client to give him the list of commands.
 *
 * @param message The message to handle 
 * @param socketClient The socket of the client who send the request of help
 *
 * @return void
 */
void handle_help_message(char* message, int socketClient) {
  int goodFormat = is_good_format_help_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Help message have to follow the format /help";
    send_response(socketClient, HELP_ERROR, message, NULL);
  }

  // Message follow the good format
  if(goodFormat == 1) {
    char* filename = "command.txt";
    char* contentCommandFile = get_content_of_file(filename);
    send_response(socketClient, HELP_SUCCESS, contentCommandFile, NULL);
  }
}



/**
 * Detect if the message corresponding to the format /help
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_help_message(char* message) {
  // Check if the string starts with "/help"
  if (strncmp(message, "/help", 5) != 0) {
    return 0;
  }

  // Check if there is no character after "/help"
  if(strlen(message + 5) != 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


/**
 * Get the content of the file specified in parameter
 *
 * @param filename The name of the file to read
 * 
 * @return The content of the file
 */
char* get_content_of_file(char *filename) {

  FILE* fp = fopen(filename, "r");

  if(fp == NULL) {
    perror("Error: opening file");
    exit(1);
  }

  // Get the size of the file
  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // Read the file into the buffer
  char* buffer = malloc(file_size + 1);
  size_t bytes_read = fread(buffer, 1, file_size, fp);
  buffer[bytes_read] = '\0';

  fclose(fp);

  return buffer;
}



/**
 * Handle send file message. Send a message to the client to confirm the send of the file.
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who send the request of send file
 *
 * @return description
 */
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
  socketFile = init_socket_file();
  connection_request(socketFile, PORT_SEND_FILE_SOCKET);

  // Create thread to receive the content of the file
  ThreadArgsRecvFile* args = malloc(sizeof(ThreadArgsRecvFile));
  args->fileName = file_name;
  args->socketFile = socketFile;
  args->sizeFile = sizeFile;

  pthread_t threadId;
  pthread_create(&threadId, NULL, thread_receive_file, (void*) args);
}


/**
 * Check if the file name and the size of the file are correct
 *
 * @param fileName The name of the file
 * @param size The size of the file
 * @param message The message to send to the client if the file name or the size of the file are not correct
 *
 * @return 1 if the file name and the size of the file are correct | 0 if not
 */
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


/**
 * Detect if the message corresponding to the format "/sendfile <file_name> <file_size>"
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
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


/**
 * Get the name and the size of the file
 *
 * @param message The string to split to get the name and the size of the file
 * @param file_name The name of the file
 * @param file_size The size of the file
 *
 * @return void
 */
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


/**
 *
 * Initialize the socket for file transfer
 *
 * @return The socket file
 */
int init_socket_file() {

  int socketFile = socket(PF_INET, SOCK_STREAM, 0);

  if(socketFile == -1) {
    perror("Error: Creation of socket");
    exit(1);
  }

  printf("Socket file created\n");
  return socketFile;
}


/**
 * Send a connection request to the new soket for file transfer
 *
 * @param socketFile The socket file
 * @param port The port of the server
 *
 * @return void
 */
void connection_request(int socketFile, int port) {
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

  printf(" => Connected to the client\n");
}


/**
 * Function thread to receive the content of the file
 *
 * @param agrs The arguments of the thread
 *
 * @return description
 */
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


/**
 * Handle list files server message. Send a message to the client to give him the list of files in the server directory.
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who send the request of list files server
 *
 * @return void
 */
void handle_list_files_server(char* message, int socketClient) {

  // Message does not follow the good format
  if(!is_good_format_list_files_server(message)) {
    char* message = "List files server message have to follow the format /listfiles server";
    send_response(socketClient, LIST_FILE_ERROR, message, NULL);
    return;
  }

  // Get the list of files
  char* listFiles = get_list_files();

  // Send the list of files to the client
  send_response(socketClient, LIST_FILE_SUCCESS, listFiles, NULL);
}


/**
 * Detect if the message corresponding to the format "/listfiles server"
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
int is_good_format_list_files_server(char* message) {
  // Check if the string starts with "/listfiles server"
  if (strncmp(message, "/listfiles server", 17) != 0) {
    return 0;
  }

  // Check if there is no character after "/listfiles server"
  if(strlen(message + 17) != 0) {
    return 0;
  }

  // The string is in the correct format
  return 1;
}


/**
 * Get the list of files in the server directory
 *
 * @return The list of files
 */
char* get_list_files() {
  DIR *directory;
  struct dirent *file;
  directory = opendir(FILE_DIRECTORY_SERVER);

  char* listFiles = malloc(RESPONSE_BUFFER_SIZE * sizeof(char));

  if(directory == NULL) {
    perror("Error: Opening directory");
    exit(1);
  }

  while((file = readdir(directory)) != NULL) {
    if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
      strcat(listFiles, file->d_name);
      strcat(listFiles, "\n");
    }
  }
  strcat(listFiles, "\0");
  closedir(directory);

  return listFiles;
}



/**
 * Handle recv file message. Send a message to the client to confirm the send of the file.
 *
 * @param message The message to handle
 * @param socketClient The socket of the client who send the request of recv file
 *
 * @return void
 */
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
  printf("File path: %s\n", filePath);
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


/**
 * Detect if the message corresponding to the format "/recvfile <file_name>"
 *
 * @param message The string to check
 *
 * @return 1 if the message is in the correct format | 0 if the message is not in the correct format
 */
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


/**
 * Return <file_name> from /recvfile <file_name> message
 *
 * @param message The message to split
 *
 * @return The name of the file
 */
char* get_file_name_recv_file_message(char* message) {
  char* token = strtok(message, " ");
  token = strtok(NULL, " ");
  return token;
}


/**
 *
 * Initialize the socket in TCP which will be used to send the file to the client
 *
 * @return The socket created
 */
int init_socket_send_file() {
	int socketServer = socket(PF_INET, SOCK_STREAM, 0);
	
  if(socketServer == -1) {
    perror("Error: Creation of socket");
    exit(1);
  }

  // Allow to use address again
  int optval = 1;
  setsockopt(socketServer, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  
  printf("Socket created\n");

  return socketServer;
}


/**
 * Name the socket for file transfer
 *
 * @param socket The socket to name
 * @param port The port of the socket
 *
 * @return void
 */
void name_socket_send_file(int socket, int port) {
  struct sockaddr_in adress;
  socklen_t sizeAdress= sizeof(adress);

  adress.sin_family = AF_INET; 
  adress.sin_addr.s_addr = INADDR_ANY;
  adress.sin_port = htons(port);

  if(bind(socket, (struct sockaddr*)&adress, sizeAdress) == -1) {
    perror("Error: Socket naming");
    exit(1);
  }
  printf(" => Named Socket successfully\n");
}


/**
 * Listen the socket for file transfer
 *
 * @return description
 */
void listen_socket_send_file(int socket) {
  if(listen(socket, 10) == -1) {
    perror("Error: Socket listening");
    exit(1);
  }
  printf(" => Socket listening\n\n");
}



/**
 * Function thread to send the name, the size and the content of the file
 *
 * @param args The arguments of the thread
 *
 * @return void
 */
void* thread_send_file(void* args) {
  ThreadArgsSendFile* data = (ThreadArgsSendFile*) args;
  char* fileName = malloc(NB_CHARACTERS * sizeof(char));
  strcpy(fileName, data->fileName);
  int socketClient = data->socket;

  free(data);

  // Create a new socket to send the file to the client
  int socketFile = init_socket_send_file();
  name_socket_send_file(socketFile, PORT_RECV_FILE_SOCKET);
  listen_socket_send_file(socketFile);


  // Send confirmation to the client
  char* responseMessage = "The server is ready to send the file";
  send_response(socketClient, SERVER_READY_FILE, responseMessage, NULL);

  // Accept the connection request
  int socketClientFile = accept(socketFile, NULL, NULL);
  printf("Connection accepted\n");

  // Get the name and the size of the file
  int sizeFile = get_size_recv_file(fileName);
  
  printf("Sending file informations (%s, %d bytes)...\n", fileName, sizeFile);

  // Send the name and the size of the file
  char* fileNameAndSize = malloc(NB_CHARACTERS * sizeof(char));
  sprintf(fileNameAndSize, "%s %d", fileName, sizeFile);
  send_response(socketClientFile, FILE_TRANSFER_SUCCESS, fileNameAndSize, NULL);

  printf("Sending file content...\n");

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

  printf("File send\n");

  fclose(file);
}


/**
 * Get the size of the file that correspond to the file name specified in parameter
 *
 * @param fileName The path of the file
 *
 * @return The size of the file
 */
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


/**
 * Function to handle the SIGINT signal (CTRL + C)
 *
 * @param signal The signal to handle
 *
 * @return void
 */
void interrupt_handler(int signal) {
  printf("\n");
  close_all_clients();
  printf("  =>  All client threads/sockets are closed\n");
  close_socket(socketServer);
  printf("  =>  Server socket is closed\n");
  deletion_semaphore();
  printf("  =>  Semaphore is deleted\n");
  printf("\n========== END OF SERVER ==========\n");
  exit(0);
}


/**
 * Close all clients thread/sockets in the list
 *
 * @return void
 */
void close_all_clients() {
  Node* currentClient = listClient->next;
  while(currentClient != listClient) {

    // Cancel the thread
    if(pthread_cancel(currentClient->thread) != 0) {
      perror("Error: Canceling the thread");
      exit(1);
    }

    // Close the socket
    close_socket(currentClient->number);

    currentClient = currentClient->next;
  }
}


/**
 * Remove the client from the list, close the socket corresponding and leave a place in the semaphore
 *
 * @param socketClient The socket of the client to remove
 *
 * @return void
 */
void remove_client(int socketClient, pthread_t threadId) {
  
  // Leave a place in the semaphore
  leave_place_semaphore();

  // Remove the client from the list
  remove_element(&listClient, search_element(&listClient, socketClient));

  // Close the socket
  close_socket(socketClient);

  printf("Client %d disconnected\n", socketClient);
  display_list(&listClient);

  // Cancel the thread
  if(pthread_cancel(threadId) != 0) {
    perror("Error: Canceling the thread");
    exit(1);
  }
}


/**
 * Receive a message from the client with the socket specified in parameter
 *
 * @param socket The socket of the client to recv the message
 * @param message The message to store the received message
 *
 * @return The number of bytes received
 */
int recv_message(int socketClient, char* message) {
  int nbByteRead = recv(socketClient, message, NB_CHARACTERS, 0);
  if(nbByteRead == -1) {
    perror("Error: Receiving the message");
    exit(1);
  }
  return nbByteRead;
}


/**
 * Send an informative, a success, a redirection or an error response of the client with the socket specified in parameter
 *
 * @param socketClient The socket of the client to send the response
 * @param code The code of the response
 * @param message The message of the response
 * @param pseudo The pseudo of the client who send the message (If the response is a redirection)
 *
 * @return 1 if the response is send | -1 if the response is not send
 */
int send_response(int socketClient, int code, char* message, char* pseudo) {
  Response* response = malloc(sizeof(Response));

  // Initialise the code field
  response->code = code;

  // Initialise the message field
  response->message = malloc(strlen(message) + 1);
  strcpy(response->message, message);

  // Initialise the from field
  if(pseudo != NULL) {
    response->from = malloc(strlen(pseudo) + 1);
    strcpy(response->from, pseudo);
  } else {
    response->from = malloc(strlen(RESPONSE_FROM_SERVER) + 1);
    strcpy(response->from, RESPONSE_FROM_SERVER);
  }

  // Serialize the response
  char buffer[RESPONSE_BUFFER_SIZE];
  serialize_response(response, buffer, sizeof(buffer));

  // Send the response
  if(send(socketClient, buffer, sizeof(buffer), 0) == -1) {
    perror("Error: Sending the response");
    return -1;
  }

  return 1;
}

/**
 * Serialize all the fields of the response into a buffer specified in parameter
 *
 * @param response The response to serialize
 * @param buffer The buffer to store the serialized response
 * @param sizeBuffer The length of the buffer
 *
 * @return void
 */
void serialize_response(Response* response, char* buffer, size_t sizeBuffer) {
  
  // Pack the code field into the buffer
  int code = htonl(response->code);
  memcpy(buffer, &code, sizeof(code));
  buffer += sizeof(code);
  sizeBuffer -= sizeof(code);

  // Pack the message field into the buffer
  size_t lengthMessage = strlen(response->message) + 1;
  if (lengthMessage > sizeBuffer) {
    perror("Error: The buffer is too small to hold the serialized message");
    exit(1);
  }
  memcpy(buffer, response->message, lengthMessage);
  buffer += lengthMessage;
  sizeBuffer -= lengthMessage;

  // Pack the from field into the buffer
  size_t lengthFrom = strlen(response->from) + 1;  // include the null terminator
  if (lengthFrom > sizeBuffer) {
    perror("Error: The buffer is too small to hold the serialized from field");
    exit(1);
  }
  memcpy(buffer, response->from, lengthFrom);
}


/**
 * Initialize the semaphore for handle capacity of clients
 *
 * @return void
 */
void init_semaphore_server() {
  int keySemaphore = ftok("command.txt", 'r');
  if (keySemaphore == -1) {
    perror("Error ftok");
    exit(1);
  }

  int nbSemaphores = 1;  
  idSemaphore = semget(keySemaphore, nbSemaphores, IPC_CREAT | 0666);
  if(idSemaphore == -1){
    perror("Error semget");
    exit(1);
  }
}


/**
 * Set the capacity of the semaphore in global variables
 *
 * @param capacity The capacity to set
 *
 * @return void
 */
void set_capacity_semaphore(int capacity) {
  union semun startnbPlacesSemaphore;
  startnbPlacesSemaphore.val = capacity;

  if (semctl(idSemaphore, 0, SETVAL, startnbPlacesSemaphore) == -1){
    perror("Error semctl");
    exit(1);
  }
  printf("Capacity: %d client(s)\n\n", startnbPlacesSemaphore.val);
}


/**
 * Deletion of the semaphore in global variables
 *
 * @return void
 */
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


/**
 * Increment the semaphore specified in parameter to leave place in it
 *
 * @return void
 */
void leave_place_semaphore() {

  struct sembuf operation = {0, 1, 0}; // Increment the semaphore

  if(semop(idSemaphore, &operation, 1) == -1) {
    perror("Error: Leaving place in the semaphore");
    exit(1);
  }

  display_remaining_places_semaphore();
}


/**
 * Display the remaining places in the semaphore
 *
 * @return void
 */
void display_remaining_places_semaphore() {
  int countPlaces = semctl(idSemaphore, 0, GETVAL);

  if(countPlaces == -1){
    perror("Error: Getting semaphore nbPlaces");
    exit(1);
  }
  printf("Remaining places: %d\n\n", countPlaces);
}
