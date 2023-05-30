#ifndef PROFANITY_MESSAGE_SERVER_H
#define PROFANITY_MESSAGE_SERVER_H

typedef struct BadWord {
  char* word;
  struct BadWord* next;
} BadWord;

// List of banned words
extern BadWord* listBadWord;

/**
 * Read the banned words from the file badWords.txt and add them to the listBadWord
 * 
 * @param listBadWord the list of banned words
 *
 * @return void
 */
extern void init_banned_words(BadWord** listBadWord);

/**
 * Display the list of banned words
 * 
 * @param listBadWord the list of banned words
 *
 * @return void
 */
extern void display_list_banned_words(BadWord** listBadWord);

/**
 * Check if the word specified in parameter is a banned word
 *
 * @param word the word to check
 * @param listBadWord the list of banned words
 *
 * @return 1 if the word is a banned word, 0 if the word is not a banned word
 */
extern int is_banned_word(char* word, BadWord** listBadWord);

/**
 * Apply the profanity filter to the message specified in parameter
 *
 * @param socketClient the socket of the client who send the message
 * @param message the message to filter
 * 
 * @return void
 */
extern void apply_profanity_filter(int socketClient, char* message);

#endif // PROFANITY_MESSAGE_SERVER_H