#include "../../../include/client/file/list_files_client.h"
#include "../../../include/client/global_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>


int is_list_file_message(char* message) {
  if(strncmp(message, "/listfiles local", 16) == 0) {
    return 1;
  } else {
    return 0;
  }
}


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