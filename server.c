#include "Practical.h"
#include "helperFunctions.h"


int main(int argc, char *argv[]) {
    int totalConnections = 0;
    int maxUsers = 2;
    int filesPerUser = 10;
    int fileCount = 0;
    char menuSelStr[3];
    int menuSelection;
    bool loop = true;
    char menuStr[] = "Select an action from the menu:\n"
        "1. Request file listing\n"
        "2. Download file\n"
        "3. List all downloads\n"
        "4. Exit\n";
    char byeStr[] = ">> Goobye!!!\n"
    
    char testFileName[] = "testFile.txt";

    int port = atoi(argv[1]); // Port number used for the connection
    char clientIP[INET_ADDRSTRLEN]; // Buffer for the client's IP address

    if (argc != 2) {
       printf("Not enough arguments, exiting\nUse <port>\n");
       return 0;
    }

    fileInfo *localFiles = listFiles("./server_files", &fileCount);

    for (int i = 0; i < fileCount; i++) {
        printf("File: %s\n", localFiles[i].fileName);
        printf("Size: %d\n", localFiles[i].fileSize);
    }

    int servSock = ServerSetup(port);
    printf("Server is listening on port %d\n", port);
    PrintLocalIP();
    int clientSock = ListenForConnections(servSock, clientIP); // Prints username and IP

    
    while (loop) {
        // Send menu to client
        SendMessage(clientSock, menuStr);

        // Receive input 1-4 from client
        // Sleep(3);
        if (ReceiveString(clientSock, menuSelStr)) // Checks received string
            menuSelection = atoi(menuSelStr);
        else
            printf("Failed to ReceiveString\n");

        switch (menuSelection) {
        case 1: // Send file list            
            printf("Server: Option selected was %s\n", menuSelStr);
            break;

        case 2: // Send filename request
            break;

        case 3: // Send download record
            break;

        case 4: // Send goodbye message
            SendMessage(clientSock, byeStr);
            close(clientSock);
            printf("Closed connection\n");
            loop = false;
        }
    }
        
    //printf("Sending file to client: %s\n", testFileName);
    
    //SendMessage(clientSock, message);

    //SendFile(clientSock, testFileName);
    return 0;
}

//Create list of local files
        //read files from directory
        //allocate memory for localFiles
        //store in localFiles

    /*// Allocate memory for the user list
    userInfo *users = malloc(maxUsers * sizeof(userInfo));
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