#ifndef HELPERFUNCTIONS_H_
#define HELPERFUNCTIONS_H_

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

#include "Practical.h"

#define MAX_BUFFER_SIZE 512 // Maximum buffer size for file content

typedef struct {
    char *fileName;
    int fileSize;
    char *fileContents;
    
} fileInfo;

typedef struct {
    char *username;
    char *IP;
    //the list of files the user has downloaded
    fileInfo *fileList;
    int numFiles;
} userInfo;


int ClientConnect(const char *serverIP, int port);

int ServerSetup(int port);

void SendFile(int sock, const char *fileName);

void SendMessage(int sock, const char *message);

//Recieves
bool ReceiveMessage(int sock);

//Prints the local IP address
void PrintLocalIP();

//Server listens for client connections
void ListenForConnections(int servSock, char *clientIP);

//Prints a local file
void ProcessFile(const char *buffer, ssize_t bufferSize, const char *fileName);

//Checks if a buffer is a file or a message
bool IsFile(const char *buffer);

#endif // HELPERFUNCTIONS_H_