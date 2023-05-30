#include "../../../include/server/help/help_server.h"
#include "../../../include/server/global_server.h"
#include "../../../include/constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int is_help_message(char* message) {
  if (strncmp(message, "/help", 5) == 0) {
    return 1;
  } else {
    return 0;
  }
}


void handle_help_message(char* message, int socketClient) {
  int goodFormat = is_good_format_help_message(message);

  // Message does not follow the good format
  if(goodFormat == 0) {
    char* message = "Help message have to follow the format /help";
    send_response(socketClient, HELP_ERROR, message, NULL);
  }

  // Message follow the good format
  if(goodFormat == 1) {
    char* filename = "./assets/command.txt";
    char* contentCommandFile = get_content_of_file(filename);
    send_response(socketClient, HELP_SUCCESS, contentCommandFile, NULL);
  }
}


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