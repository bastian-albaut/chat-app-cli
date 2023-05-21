#ifndef LIST_FILES_CLIENT_H
#define LIST_FILES_CLIENT_H

/**
 * Detect if the message corresponding to list file command (start with "/listfiles")
 *
 * @param message The message to check
 *
 * @return 1 if the message is a list file command | 0 if the message is not a list file command
 */
extern int is_list_file_message(char* message);

/**
 * Display the list of files of the directory FILE_DIRECTORY_CLIENT
 *
 * @return void
 */
extern void handle_list_file_message(char* message);

/**
 * Detect if the message corresponding to the format /listfiles local
 *
 * @param param description
 *
 * @return description
 */
extern int is_good_format_list_file_message(char* message);

#endif // LIST_FILES_CLIENT_H