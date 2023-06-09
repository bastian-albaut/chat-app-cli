#include "../../../include/server/message/profanity_filter_server.h"
#include "../../../include/constants.h"
#include "../../../include/server/global_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


BadWord* listBadWord = NULL;

void init_banned_words(BadWord** listBadWord) {
    // Open the file badWords.txt
    FILE* file = fopen("./assets/badWords.txt", "r");
    if(file == NULL) {
        printf("Error: cannot open the file badWords.txt\n");
        exit(1);
    }

    // Read the file line by line
    char* line = NULL;
    size_t length = 0;
    ssize_t read;
    while ((read = getline(&line, &length, file)) != -1) {
        
        // Remove the \n at the end of the line
        char* removeFound = strchr(line, '\n');
        if(removeFound != NULL) {
            *removeFound = '\0';
        }

        // Add the word to the listBadWord
        BadWord* newBadWord = malloc(sizeof(BadWord));
        newBadWord->word = malloc(strlen(line) * sizeof(char));
        strcpy(newBadWord->word, line);
        newBadWord->next = *listBadWord;
        *listBadWord = newBadWord;
    }

    // Close the file
    fclose(file);
}

void display_list_banned_words(BadWord** listBadWord) {
    BadWord* currentBadWord = *listBadWord;
    printf("========== List of banned word(s) ==========\n");
    while (currentBadWord != NULL) {
        printf("%s\n", currentBadWord->word);
        currentBadWord = currentBadWord->next;
    }
    printf("================= End list =================\n\n");
}

int is_banned_word(char* word, BadWord** listBadWord) {
    // Convert the current word to lowercase
    char lowercaseWord[NB_CHARACTERS];
    for (int i = 0; word[i] != '\0'; ++i) {
        lowercaseWord[i] = tolower(word[i]);
    }
    lowercaseWord[strlen(word)] = '\0';

    // Check if any banned word is a substring of the lowercase word
    BadWord* currentBadWord = *listBadWord;
    while (currentBadWord != NULL) {
        // Convert the banned word to lowercase
        char lowercaseBadWord[NB_CHARACTERS];
        for (int i = 0; currentBadWord->word[i] != '\0'; ++i) {
            lowercaseBadWord[i] = tolower(currentBadWord->word[i]);
        }
        lowercaseBadWord[strlen(currentBadWord->word)] = '\0';

        if (strstr(lowercaseWord, lowercaseBadWord) != NULL) {
            return 1;
        }
        currentBadWord = currentBadWord->next;
    }
    return 0;
}


int apply_profanity_filter(int socketClient, char* message) {
    char filteredMessage[NB_CHARACTERS]; // New string to store the filtered message
    char* word = strtok(message, " ");
    int isFirstWord = 1;
    int isBannedWord = 0;

    while (word != NULL) {
        if (is_banned_word(word, &listBadWord)) {
            // Replace banned word with asterisks (*)
            memset(word, '*', strlen(word));
            isBannedWord = 1;
        }

        if (!isFirstWord) {
            strcat(filteredMessage, " "); // Append a space after each word (except the first word)
        } else {
            isFirstWord = 0;
        }

        strcat(filteredMessage, word); // Append the filtered word to the filtered message

        word = strtok(NULL, " ");
    }

    strcpy(message, filteredMessage); // Copy the filtered message back to the original message

    if (isBannedWord) {
        char* responseMessage = "Your message contains banned words";
        send_response(socketClient, FILTER_MESSAGE, responseMessage, NULL);
        return 1;
    }

    return 0;
}