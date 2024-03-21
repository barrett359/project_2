#include "Practical.h"
#include "helperFunctions.h"


int main(int argc, char *argv[]) {
    int totalConnections = 0;
    int maxUsers = 2;
    int filesPerUser = 10;
    
    fileInfo *localFiles;

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

    int port = atoi(argv[1]); // Port number used for the connection
    char *clientIP;

    int ServerSocket = ServerSetup(atoi(argv[1]));
    printf("Server is listening on port %d\n", port);
    PrintLocalIP();
    clientIP = ListenForConnections(port);
    printf("Connection from %s\n", clientIP);
    SendMessage(ServerSocket, "Hello World");
    close(ServerSocket);
    printf("Server closed connection\n");
    
    //while (totalConnections < 2) {
        //listen for connections
        //store username and IP in users
        //totalConnections++
    //}


   /*free(users->fileList);
    free(users);*/
    
    return 0;
}