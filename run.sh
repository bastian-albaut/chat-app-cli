#!/bin/bash

# Compile client.c and server.c
gcc client.c -o client
gcc server.c -o server

# Display message to let the user know how to stop the processes
echo "Type 'stop' in terminal window to stop the process."

# Open another new terminal window and run server
if gnome-terminal --title="Server" -- bash -c "./server 5324"; then
    echo "Server launch successful"

else 
    echo "Error: Launch server program"
    exit 1
fi

# Open a new terminal window and run client
if gnome-terminal --title="Client" -- bash -c "./client 127.0.0.1 5324"; then
    echo "Client launch successful"
else 
    echo "Error: Launch client pro"
fi