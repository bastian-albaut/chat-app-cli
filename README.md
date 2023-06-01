# Chat Server/Client in cli

This project is an instant messaging that allow multiple clients to communicate, share files or be part of chat rooms...

## Features
- Choose a pseudonym
- Send a global message to all clients
- Send a private message to a specific user
- Upload a file to the server
- Download a file from the server 
- Join / Quit a chat room
- Create / Delete a chat room
- Censorship of bad words

## Personalisation
You can update the list of bad words in the file badWords.txt
The file is located at `./assets/badWords.txt`.

## Technical informations
- Server side the clients are store with a Doubly Circular Linked Lists with head of list
- RW lock for concurent access in read and exclusive access in write
- A thread is created on the server side when the client connects to manage it 

## Run 
Enter the following command in a terminal:
```
./run.sh
```
