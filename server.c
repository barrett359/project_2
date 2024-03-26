#include "Practical.h"
#include "helperFunctions.h"
#include <string.h>

int main(int argc, char *argv[]) {
    int maxFilesPerUser = 10;
    int fileCount = 0;

    char filename[50];
    char menuSelStr[3];
    int menuSelection;
    
    char menuStr[] = "Select an action from the menu:\n"
        "1. Request file listing\n"
        "2. Download file\n"
        "3. List all downloads\n"
        "4. Exit\n";
    
    char testFileName[] = "testFile.txt";
    char clientName[20];
    
    int port = atoi(argv[1]); // Port number used for the connection
    char clientIP[INET_ADDRSTRLEN]; // Buffer for the client's IP address
    int uniqueUsers = 0;//used to store the max index of users that ha
    int maxConnections = 10; //used to store the index of the users that have been stored in the array
    int currentUser = 0; //used to store the index of the current user

    if (argc != 2) {
       printf("Not enough arguments, exiting\nUse <port>\n");
       return 0;
    }
    

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
    

    fileInfo *localFiles = listFiles("./server_files", &fileCount);

    for (int i = 0; i < fileCount; i++) {
        printf("File: %s\n", localFiles[i].fileName);
        printf("Size: %d\n", localFiles[i].fileSize);
    }

    int servSock = ServerSetup(port);
    // printf("Server is listening on port %d\n", port);
    PrintLocalIP();
    
    while (uniqueUsers < maxConnections) {
        int clientSock = ListenForConnections(servSock, clientIP,  clientName); // Prints username and IP
        bool loop = true;

        
        for (int i = 0; i < uniqueUsers; i++) {
            printf("User %d: %s\n", i, users[i].username);
            printf("IP: %s\n", users[i].IP);
            printf("Number of files: %d\n", users[i].numFiles);
            for (int j = 0; j < users[i].numFiles; j++) {
                printf("File %d: %s\n", j, users[i].fileList[j].fileName);
                printf("Size: %d\n", users[i].fileList[j].fileSize);
            }
        }

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
        

        users[0].username = "johnny";
        users[0].IP = "";
        users[0].numFiles = 3;
        users[0].fileList[0].fileName = "file1.txt";
        users[0].fileList[0].fileSize = 256;
        users[0].fileList[1].fileName = "file2.txt";
        users[0].fileList[1].fileSize = 512;
        users[0].fileList[2].fileName = "file3.txt";
        users[0].fileList[2].fileSize = 1024;
    
        while (loop) {
            // Send menu to client
            SendMessage(clientSock, menuStr);

            // Receive input 1-4 from client
            // Sleep(3);
            if (ReceiveString(clientSock, menuSelStr)) // Checks received string
                menuSelection = atoi(menuSelStr);
            else
                printf("Failed to Receive String\n");

        switch (menuSelection) {
        case 1: // Send file list         
            printf("\t%s requested file listing\n", clientName);
            SendMessage(clientSock, "File List\n");
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
                }
            }
            else
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
                int length = 0; // Initialize length to 0
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
    close(servSock);
    
    
    return 0;
}

//Create list of local files
        //read files from directory
        //allocate memory for localFiles
        //store in localFiles

    /*// Allocate memory for the user list
    userInfo *users = malloc(maxConnections * sizeof(userInfo));
    if (users == NULL) {
        fprintf(stderr, "Failed to allocate memory for users\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for user specific file lists
    for (int i = 0; i < 2; i++) {
        users[i].fileList = malloc(filesPerUser * sizeof(fileInfo));
        if (users[i].fileList == NULL || users) {
            fprintf(stderr, "Failed to allocate memory for file list\n");
            exit(EXIT_FAILURE);
        }
    }*/

//while (totalConnections < 2) {
        //listen for connections
        //store username and IP in users
        //totalConnections++
    //}


   /*free(users->fileList);
    free(users);*/