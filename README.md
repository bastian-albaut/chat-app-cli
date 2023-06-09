# Chat App in CLI

This project is a fast and reliable instant messaging that allow multiple clients to communicate, share files or be part of chat rooms...

![Screenshot](https://github.com/bastian-albaut/Chat-Server-Client-Cli/blob/main/docs/images/headerReadme.png)
*Example of a communication between two clients*

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
- Multi-threading to manage multiple clients, handle file transfer...

## Installation and Usage
1. Clone the repository:
```bash
git clone https://github.com/bastian-albaut/chat-app-cli.git
```
2. Enter the following command on root of the project:
```bash
./run.sh
```



