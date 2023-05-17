#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NB_CHARACTERS 128
#define NB_CHARACTERS_PSEUDO_MAX 20
#define NB_CHARACTERS_PSEUDO_MIN 4
#define RESPONSE_BUFFER_SIZE 10000

#define CAPACITY_CLIENTS 2

/* Code for the response */

#define NO_OTHER_USERS 100

#define PSEUDO_ACCEPTED 200
#define GLOBAL_MESSAGE_SEND 201
#define PRIVATE_MESSAGE_SEND 202
#define DISCONNECT_SUCCESS 203
#define HELP_SUCCESS 204
#define REQUEST_SEND_FILE_ACCEPTED 205
#define SERVER_READY_FILE 206
#define FILE_TRANSFER_SUCCESS 207
#define LIST_FILE_SUCCESS 207

#define MESSAGE_GLOBAL_REDIRECT 300
#define MESSAGE_PRIVATE_REDIRECT 301

#define PSEUDO_BAD_FORMAT 400
#define PSEUDO_ALREADY_USED 400
#define COMMAND_NOT_FOUND 402
#define GLOBAL_MESSAGE_ERROR 403
#define GLOBAL_MESSAGE_BAD_FORMAT 404
#define PRIVATE_MESSAGE_BAD_FORMAT 405
#define PRIVATE_MESSAGE_USER_NOT_FOUND 406
#define DISCONNECT_BAD_FORMAT 407
#define HELP_ERROR 408
#define SEND_FILE_ERROR 409
#define LIST_FILE_ERROR 410


#define RESPONSE_FROM_SERVER "Server"

#define FILE_DIRECTORY_CLIENT "files/client/"
#define FILE_DIRECTORY_SERVER "files/server/"
#define MAX_SIZE_FILE 1000000

#define PORT_FILE_SOCKET 5000

/* Color for print */
#define BLUE "\x1B[34m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define RED "\x1B[31m"
#define BOLD "\e[1m"
#define RESET "\x1B[0m"

#endif // CONSTANTS_H
