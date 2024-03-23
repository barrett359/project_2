#include "Practical.h"
#include "helperFunctions.h"
#include <stdio.h> // For user input

//
int main(int argc, char *argv[]) {
    if (argc != 3) {
       printf("Not enough arguments, exiting\nUse <IP> <port>\n");
       return 0;
    }
    char *serverIP = argv[1];
    int port = atoi(argv[2]);
    char username[20];

    // Ask for name
    printf(">> Please enter your username\n>> ");
    scanf("%s", username);

    // Server socket
    int serverSocket = ClientConnect(serverIP, port);

    // Send username to server
    SendMessage(serverSocket, username);
    
    //printf("Server Socket: %d\n", serverSocket);
    ReceiveMessage(serverSocket);
    return 0;
}