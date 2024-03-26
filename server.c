#include "Practical.h"
#include "helperFunctions.h"


int main(int argc, char *argv[]) {
    int maxFilesPerUser = 10;
    int fileCount = 0;

    char menuSelStr[3];
    int menuSelection;
    char menuString[] = "Select an actin from the menu:\n"
        "1. Request file listing\n"
        "2. Download file\n"
        "3. List all downloads\n"
        "4. Exit\n";
    
    char testFileName[] = "testFile.txt";
    char username[20];
    
    int port = atoi(argv[1]); // Port number used for the connection
    char clientIP[INET_ADDRSTRLEN]; // Buffer for the client's IP address
    int totalConnections = 0;
    int maxConnections = 2;

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
    printf("Server is listening on port %d\n", port);
    PrintLocalIP();
    
    int clientSock = ListenForConnections(servSock, clientIP,  username); // Prints username and IP

    //save username and IP in users
    if (!checkUser(username, users, totalConnections)){
        users[totalConnections].username = username;
        users[totalConnections].IP = clientIP;
        totalConnections++;
        printf("User not found\n adding user.\n");
    } else {
        printf("User found\n");
    }

    writeUserInfoToFile(users, totalConnections, "users.txt");   
    
    // Send menu to client
    SendMessage(clientSock, menuString);
    
    // Receive input 1-4 from client
    // Sleep(3);
    if (ReceiveString(clientSock, menuSelStr)) // Checks received string
        menuSelection = atoi(menuSelStr);
    else
        printf("Failed to Receive String\n");

    // Switch case?
    /*switch (menuSelection) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            return 0;
    }*/

    printf("Server: Option selected was %s\n", menuSelStr);
        
    //printf("Sending file to client: %s\n", testFileName);
    
    //SendMessage(clientSock, message);

    //SendFile(clientSock, testFileName);

    close(clientSock);
    printf("Closed connection\n");
    
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