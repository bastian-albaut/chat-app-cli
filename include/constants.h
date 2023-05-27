#ifndef CONSTANTS_H
#define CONSTANTS_H

#define NB_CHARACTERS 128
#define NB_CHARACTERS_PSEUDO_MAX 20
#define NB_CHARACTERS_PSEUDO_MIN 4
#define RESPONSE_BUFFER_SIZE 10000

#define CAPACITY_CLIENTS 2

/* Enumeration of code for response */
typedef enum ErrorCode {
  // Informative response
  NO_OTHER_USERS = 100,

  // Success response
  PSEUDO_ACCEPTED = 200,
  GLOBAL_MESSAGE_SEND = 201,
  PRIVATE_MESSAGE_SEND = 202,
  DISCONNECT_SUCCESS = 203,
  HELP_SUCCESS = 204,
  REQUEST_SEND_FILE_ACCEPTED = 205,
  SERVER_READY_FILE = 206,
  FILE_TRANSFER_SUCCESS = 207,
  LIST_FILE_SUCCESS = 208,
  RECV_FILE_SUCCESS = 209,
  LIST_ROOM_SUCCESS = 210,
  JOIN_ROOM_SUCCESS = 211,

  // Redirect response
  MESSAGE_GLOBAL_REDIRECT = 300,
  MESSAGE_PRIVATE_REDIRECT = 301,

  // Error response
  PSEUDO_BAD_FORMAT = 400,
  PSEUDO_ALREADY_USED = 400,
  COMMAND_NOT_FOUND = 402,
  GLOBAL_MESSAGE_ERROR = 403,
  GLOBAL_MESSAGE_BAD_FORMAT = 404,
  PRIVATE_MESSAGE_BAD_FORMAT = 405,
  PRIVATE_MESSAGE_USER_NOT_FOUND = 406,
  DISCONNECT_BAD_FORMAT = 407,
  HELP_ERROR = 408,
  SEND_FILE_ERROR = 409,
  LIST_FILE_ERROR = 410,
  RECV_FILE_ERROR = 411,
  LIST_ROOM_ERROR = 412,
  JOIN_ROOM_ERROR = 413,
} ErrorCode;


#define RESPONSE_FROM_SERVER "Server"

// File transfer
#define FILE_DIRECTORY_CLIENT "files/client/"
#define FILE_DIRECTORY_SERVER "files/server/"
#define MAX_SIZE_FILE 1000000
#define IP_LOCAL "127.0.0.1"
#define PORT_SEND_FILE_SOCKET 5000
#define PORT_RECV_FILE_SOCKET 6000

// Room chat
#define MAX_NB_ROOM 10
#define MAX_NB_CLIENT 10

/* Color for print */
#define BLUE "\x1B[34m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define RED "\x1B[31m"
#define GREY "\x1B[90m"
#define BOLD "\x1B[1m"
#define RESET "\x1B[0m"

#endif // CONSTANTS_H
