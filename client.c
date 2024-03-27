/*
*	James Williams
*   Bryan Molina Cervantes
*	Assignment 2
*
*	This program is designed to simulate a simple file server.
*	It will send files to corresponding clients and allowing them to view a list of available files as well as their downloaded ones.
* 	The server will host up to 2 connections before terminating.
*/

#include "Practical.h"
#include "helperFunctions.h"
#include <stdio.h> // For user input
#include <ctype.h> // For isdigit()

//
int main(int argc, char *argv[]) {
    if (argc < 3) {
       printf("Not enough arguments, exiting\nUse <IP> <port>\n");
       return 0;
    }
    char filePath[100] = "./client_files";
    // Check if a directory is provided
    if (argc == 4) {
        char *dir = argv[3];
        snprintf(filePath, sizeof(filePath), "%s%s", "./", dir);
        printf("Directory Selected: %s\n", filePath);
    }else{
        printf("No directory selected, using default: %s\n", filePath);
        printf("To select a directory, use <IP> <port> <directory>\n");
    }
    //In either case, check if the directory exists, if not, create it
    if(CreateDirectoryIfNotExist(filePath)!=0) {
        printf("Failed to create directory, exiting\n");
        return 0;
    }

    char *serverIP = argv[1];
    int port = atoi(argv[2]);
    char username[20];
    char filename[50];
    int menuSelection = -1;
    char serverFileRespStr[3];
    int serverFileResp = -1;
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

        // Send menu selection to server
        SendOption(serverSocket, menuSelection);
        if (menuSelection == 4) {
            loop = false;
        }

        // Switch case for menu selection
        switch (menuSelection) {
        case 1: // Receive file list            
            ReceiveMessage(serverSocket);
            break;

        case 2: // Receive filename request
            ReceiveMessage(serverSocket);
            printf(">> ");
            scanf("%s", filename);
            SendMessage(serverSocket, filename);

            // Checks response 1 or 2
            if (ReceiveString(serverSocket, serverFileRespStr)) 
                serverFileResp = atoi(serverFileRespStr);
            else
                printf("Failed to ReceiveString\n");
            
            // Option 2 - downloads file and displays it
            if (serverFileResp == 2) {
                // Receives file and saves it
                ReceiveNamedFile(serverSocket, filename, filePath);
                printf("\n\n");
            }

            // Option 1 - File not found, server sends nothing
            else { 
                printf("File not found!\n");
            }
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
    close(serverSocket);
    
    //printf("Server Socket: %d\n", serverSocket);
    return 0;
}