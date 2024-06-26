#include "Practical.h"
#include "helperFunctions.h"
#include <string.h>

int main(int argc, char *argv[]) {

    // Check for correct number of arguments
    if (argc < 2) {
       printf("Not enough arguments, exiting\nUse <port>\n");
       return 0;
    }
    
    char filePath[100] = "./server_files";

    printf("Welcome to Simple File Server\n");
    printf("Hint: To exit early, join as user 'exit'\n");

    // Check if a directory is provided
    if (argc == 3) {
        char *dir = argv[2];
        snprintf(filePath, sizeof(filePath), "%s%s", "./", dir);
        printf("Directory Selected: %s\n", filePath);
    }
    // If no directory is provided, use the default
    else{
        printf("No directory selected, using default: %s\n", filePath);
        printf("To select a directory, use <port> <directory>\n");
    }
    //in either case, check if the directory exists, if not, create it
    if(CreateDirectoryIfNotExist(filePath)!=0) {
        printf("Failed to create directory, exiting\n");
        return 0;
    }

    //Maximum number of files per user
    int maxFilesPerUser = 10;
    // Number of files in the server_files directory
    int fileCount = 0;
    // Buffer for the filename from the client
    char filename[20];
    // Buffer for the menu selection from the client
    char menuSelStr[3];
    //Used to store menu selection from the client
    int menuSelection;
    //Menu to be sent to the client
    char menuStr[] = "Select an action from the menu:\n"
                    "1. Request file listing\n"
                    "2. Download file\n"
                    "3. List all downloads\n"
                    "4. Exit\n";

    // Buffer for the client's username. Later stored in users
    char clientName[20];
    // Port number used for the connection
    int port = atoi(argv[1]); 
    // Buffer for the client's IP address
    char clientIP[INET_ADDRSTRLEN]; 
    // Number of unique users that have connected to the server
    int uniqueUsers = 0;
    // Maximum number of unique users that can connect before the server exits.
    int maxConnections = 10; 
    //used to store the index of the current user
    int currentUser = 0;
    
    // Allocate memory for the users array
    userInfo *users = malloc((maxConnections) * sizeof(userInfo));
    if (users == NULL) {
        fprintf(stderr, "Failed to allocate memory for users\n");
        exit(EXIT_FAILURE);
    }

    //Allocate memory for the fileList for each user
    for (int i = 0; i < maxConnections; i++) {
        users[i].fileList = malloc(maxFilesPerUser * sizeof(fileInfo));
        if (users[i].fileList == NULL) {
            fprintf(stderr, "Failed to allocate memory for fileList of user %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Get the list of files in the server_files directory
    fileInfo *localFiles = listFiles(filePath, &fileCount);

    // Print the list of files (testing purposes)
    /*for (int i = 0; i < fileCount; i++) {
        printf("File: %s\n", localFiles[i].fileName);
        printf("Size: %d\n", localFiles[i].fileSize);
    }*/

    // Set up the server
    int servSock = ServerSetup(port);
    
    //Prints the server's IP address to make client connection easier
    PrintLocalIP();
    
    // Loop to accept connections. Will run maxConnections times.
    while (uniqueUsers < maxConnections) {
        int clientSock = ListenForConnections(servSock, clientIP,  clientName); // Prints username and IP
        bool loop = true;
        
        bool userExists = false;
        currentUser = checkUser(clientName, users, uniqueUsers, &userExists);

        if (!userExists) {
            //Save username and IP in users
            currentUser = uniqueUsers;
            users[uniqueUsers].username = malloc(strlen(clientName) + 1);
            strcpy(users[uniqueUsers].username, clientName);

            users[uniqueUsers].IP = malloc(strlen(clientIP) + 1);
            strcpy(users[uniqueUsers].IP, clientIP);
            uniqueUsers++;
        }

        //Prints the entire users array (testing purposes)
        /*for (int i = 0; i < uniqueUsers; i++) {
            printf("User %d: %s\n", i, users[i].username);
            printf("IP: %s\n", users[i].IP);
            printf("Number of files: %d\n", users[i].numFiles);
            for (int j = 0; j < users[i].numFiles; j++) {
                printf("File %d: %s\n", j, users[i].fileList[j].fileName);
                printf("Size: %d\n", users[i].fileList[j].fileSize);
            }
        }*/

        while (loop) {
            // Send menu to client
            sleep(3);
            SendMessage(clientSock, menuStr);

            // Receive input 1-4 from client
            if (ReceiveString(clientSock, menuSelStr)) // Checks received string
                menuSelection = atoi(menuSelStr);
            else
                printf("Failed to Receive String\n");

            switch (menuSelection) {
                case 1: // Send file list
                    printf("\t%s requested file listing\n", clientName);

                    char message[MAX_BUFFER_SIZE];// Buffer for the message
                    strcpy(message, "Filename\tSize\n"); // Header for the message
                    int length = strlen(message); // Initial length of the message

                    for (int i = 0; i < fileCount; i++) {
                        // Calculate remaining buffer space
                        int bufferSpace = MAX_BUFFER_SIZE - length - 1; // -1 for null terminator
                        // Check if there's enough buffer space for the next line
                        if (bufferSpace > 0) {
                            int lineLength = snprintf(NULL, 0, "%s %d bytes\n", localFiles[i].fileName, localFiles[i].fileSize);
                            // If enough space, append to message; otherwise, break the loop
                            if (lineLength < bufferSpace) {
                                snprintf(message + length, bufferSpace, "%s %d bytes\n", localFiles[i].fileName, localFiles[i].fileSize);
                                length += lineLength;
                            } else {
                                fprintf(stderr, "Buffer full, some files may not be listed\n");
                                break;
                            }
                        }
                    }
                    SendMessage(clientSock, message);

                    break;

                case 2: // Send file to download
                    SendMessage(clientSock, ">> Enter filename you would like to download:\n");

                    if (ReceiveString(clientSock, filename)) { // Receive file name
                        printf("\t%s requested download: %s\n", clientName, filename);
                        int fileIndex = -1; 

                            // Search if file exists.
                            for (int i = 0; i < fileCount; i++) {
                                if (strcmp(localFiles[i].fileName, filename) == 0) {
                                    fileIndex = i;
                                    break;
                                }
                            }

                            // File not found, server only sends response
                            if (fileIndex == -1) {
                                printf("File not found!\n");
                                SendOption(clientSock, 1); // Option 1 sent
                            }

                            // Sends response, sends file
                            else {
                                SendOption(clientSock, 2); // Option 2 sent
                                SendFile(clientSock, filename);
                                printf("Sent %s to %s\n", filename, clientName);

                                // Save file name and size in user's fileList
                                users[currentUser].fileList[users[currentUser].numFiles].fileName = strdup(filename); 
                                users[currentUser].fileList[users[currentUser].numFiles].fileSize = localFiles[fileIndex].fileSize;
                                users[currentUser].numFiles++;
                                
                            }
                        
                    } else
                        printf("Failed to receive filename!\n"); 
                    break;

                case 3: // Send list of all downloads
                    printf("%s requested a list of all downloads\n", users[currentUser].username);

                    char *downloadList = malloc(MAX_BUFFER_SIZE); // Allocate buffer for the download list
                    if (downloadList == NULL) {
                        // Handle allocation failure
                        fprintf(stderr, "Memory allocation failed\n");
                        exit(EXIT_FAILURE);
                    }

                    // Properly initialize the buffer
                    downloadList[0] = '\0';

                    // Start building the download list string
                    length = 0; // reset length
                    for (int i = 0; i < uniqueUsers && length < MAX_BUFFER_SIZE - 1; i++) {
                        // Check to ensure at least one file is there for the user
                        int USERNAME_WIDTH = strlen(users[i].username);
                        if(users[i].numFiles > 0) {
                            char justifiedUsername[USERNAME_WIDTH + 1]; // +1 for null terminator
                            snprintf(justifiedUsername, sizeof(justifiedUsername), "%*s", -USERNAME_WIDTH, users[i].username); // Right-justify the username

                            // For the first file of each user, print the user name
                            length += snprintf(downloadList + length, MAX_BUFFER_SIZE - length, "%s %s %d bytes\n",
                                            justifiedUsername, users[i].fileList[0].fileName, users[i].fileList[0].fileSize);
                        }

                        // Now, handle subsequent files for the same user
                        for (int j = 1; j < users[i].numFiles && length < MAX_BUFFER_SIZE - 1; j++) {
                            length += snprintf(downloadList + length, MAX_BUFFER_SIZE - length, "%*s%s %d bytes\n",
                                            USERNAME_WIDTH, "", users[i].fileList[j].fileName, users[i].fileList[j].fileSize);
                        }
                    }
                    if(downloadList[0] == '\0') {
                        snprintf(downloadList, MAX_BUFFER_SIZE, "No downloads yet\n");
                    }
                    // Send the download list to the client
                    SendMessage(clientSock, downloadList);

                    break;

                case 4: // Send goodbye message
                    SendMessage(clientSock, ">> Goobye!!!\n");
                    close(clientSock);
                    printf("Connection with %s terminated\n", clientName);
                    loop = false;
            }
            
        }
        
        // Exit if the user is 'exit'
        if(strcmp(clientName, "exit")==0){
            printf("Exit override received\nExiting...\n");
            break;
        }
    }

    free(users);
    free(localFiles);
    
    close(servSock);

    return 0;
}

