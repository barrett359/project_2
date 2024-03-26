#include "Practical.h"
#include "helperFunctions.h"
#include <stdio.h> // For user input
#include <ctype.h> // For isdigit()

//
int main(int argc, char *argv[]) {
    if (argc != 3) {
       printf("Not enough arguments, exiting\nUse <IP> <port>\n");
       return 0;
    }
    char *serverIP = argv[1];
    int port = atoi(argv[2]);
    char username[20];
    int menuSelection = -1;
    bool loop = true;

    // Ask for name
    printf(">> Please enter your username\n>> ");
    scanf("%s", username);

    // Server socket
    int serverSocket = ClientConnect(serverIP, port);

    // Send username to server
    SendMessage(serverSocket, username);
    
    while (loop) {
        // Receive menu from server
        ReceiveMessage(serverSocket);
        menuSelection = -1; // menuSelection reset
        
        while (menuSelection == -1) {
            printf(">> ");
            if (scanf("%d", &menuSelection) != 1) {
                printf("Please enter a valid number.\n");
                while (getchar() != '\n'); // Clear input buffer
            }
            else {
                if (menuSelection < 1 || menuSelection > 4) {
                    printf("Please enter a value between 1-4.\n");
                    menuSelection = -1; // Reset menuSelection
                }
            }
        }

        // Send input as int or convert to str?
        // **SendMessage(serverSocket, menuSelection); 
        SendOption(serverSocket, menuSelection);
        if (menuSelection == 4) {
            loop = false;
        }

        switch (menuSelection) {
        case 1: // Receive file list            
            ReceiveMessage(serverSocket);
            break;

        case 2: // Receive filename request
            ReceiveMessage(serverSocket);
            break;

        case 3: // Receive download record
            ReceiveMessage(serverSocket);
            break;

        case 4: // Receive goodbye message
            ReceiveMessage(serverSocket);
            loop = false;
            // return 0; // Can end client here or stop look then end. Whichever you prefer
        }
    }
    
    //printf("Server Socket: %d\n", serverSocket);
    return 0;
}