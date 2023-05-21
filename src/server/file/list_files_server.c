#include "../../../include/server/file/list_files_server.h"
#include "../../../include/server/global_server.h"
#include "../../../include/constants.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

int is_list_files_server(char* message) {
  if (strncmp(message, "/listfiles server", 17) == 0) {
    return 1;
  } else {
    return 0;
  }
}


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