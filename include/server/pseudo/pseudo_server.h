#ifndef PSEUDO_SERVER_H
#define PSEUDO_SERVER_H

/**
 * Handle the pseudo of the client
 *
 * @param socketClient The socket of the client which want to get a pseudo
 *
 * @return 1 if handle pseudo work correctly | -1 if not
 */
extern int handle_pseudo_client(int socketClient, char* pseudo);

/**
 * Get pseudo of the client and check if it is already used
 *
 * @param socketClient The socket of the client which want to get a pseudo
 *
 * @return The pseudo of the client
 */
extern char* get_pseudo(int socketClient);

/**
 * Check if the pseudo is in the good format
 *
 * @param pseudo The pseudo to check
 *
 * @return 1 if the pseudo is in the good format | 0 if is not in the good format
 */
extern int check_format_pseudo(char* pseudo, char* responseMessage);

#endif // PSEUDO_SERVER_H