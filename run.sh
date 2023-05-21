#!/bin/bash

# Compile client.c and server.c
gcc -o exClient ./src/client.c -lpthread ./src/global.c ./src/client/global_client.c ./src/client/semaphore/semaphore_client.c ./src/client/file/list_files_client.c ./src/client/file/recv_file_client.c ./src/client/file/send_file_client.c
gcc -o exServer ./src/server.c -lpthread ./src/global.c ./lib/list.c ./src/server/global_server.c ./src/server/semaphore/semaphore_server.c ./src/server/file/list_files_server.c ./src/server/file/download_file_server.c ./src/server/file/upload_file_server.c ./src/server/help/help_server.c ./src/server/logout/logout_server.c ./src/server/message/global_message_server.c ./src/server/message/private_message_server.c ./src/server/pseudo/pseudo_server.c 

# Open another new terminal window and run server
if gnome-terminal --title="Server" -- bash -c "./exServer 7000"; then
    echo "Server launch successful"

else 
    echo "Error: Launch Server"
    exit 1
fi

# Open a new terminal window and run client 1
if gnome-terminal --title="Client 1" -- bash -c "./exClient 127.0.0.1 7000"; then
    echo "Client 1 launch successful"
else 
    echo "Error: Launch Client 1"
fi

# Open a new terminal window and run client 2
if gnome-terminal --title="Client 2" -- bash -c "./exClient 127.0.0.1 7000"; then
    echo "Client 2 launch successful"
else 
    echo "Error: Launch Client 2"
fi