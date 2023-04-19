#!/bin/bash

# Compile client.c and server.c
gcc -o client client.c -lpthread
gcc -o server server.c -lpthread ./utils/list.c

# Open another new terminal window and run server
if gnome-terminal --title="Server" -- bash -c "./server 5000"; then
    echo "Server launch successful"

else 
    echo "Error: Launch Server"
    exit 1
fi

# Open a new terminal window and run client 1
if gnome-terminal --title="Client 1" -- bash -c "./client 127.0.0.1 5000"; then
    echo "Client 1 launch successful"
else 
    echo "Error: Launch Client 1"
fi

# Open a new terminal window and run client 2
if gnome-terminal --title="Client 2" -- bash -c "./client 127.0.0.1 5000"; then
    echo "Client 2 launch successful"
else 
    echo "Error: Launch Client 2"
fi