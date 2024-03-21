#include "Practical.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Should be in .h file?
typedef struct {
    char *fileName;
    int fileSize;
    char *fileContents;
    
} fileInfo;

// Should be in .h file?
typedef struct {
    char *username;
    char *IP;
    fileInfo *fileList;
    int numFiles;
} userInfo;


int main(int argc, char *argv[]) {
    int totalConnections = 0;
    int maxUsers = 2;
    int filesPerUser = 10;
    
    fileInfo *localFiles;

    //Create list of local files
        //read files from directory
        //allocate memory for localFiles
        //store in localFiles

    // Allocate memory for the user list
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
    }




    //start server
    
    //while (totalConnections < 2) {
        //listen for connections
        //store username and IP in users
        //totalConnections++
        
    //}


    free(users->fileList);
    free(users);
    
    return 0;
}