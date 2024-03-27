/*
*	James Williams
*   Bryan Molina Cervantes
*	Assignment 2
*
*	This program is designed to simulate a simple file server.
*	It will send files to corresponding clients and allowing them to view a list of available files as well as their downloaded ones.
* 	The server will host up to 2 connections before terminating.
*/

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
#include <dirent.h>
#include <sys/stat.h>

#include "Practical.h"

#define MAX_BUFFER_SIZE 2000 // Maximum buffer size for file content

typedef struct {
    char *fileName;
    int fileSize;
} fileInfo;

typedef struct {
    char *username;
    char *IP;
    //list of the files the user has downloaded
    fileInfo *fileList;
    int numFiles;
} userInfo;

/**
* Brief: Connects to a server.
* @param serverIP IP address of the server to connect to.
* @param port Port number of the server to connect to.
* @return The socket file descriptor.
* 
* Creates a client socket then connects to a server 
* with the given IP address and port number
*/
int ClientConnect(const char *serverIP, int port);

/**
* Brief: Sets up a server.
* @param port Port number to listen on.
* @return The server socket file descriptor.
*
* Creates a server socket then listens on the given port number.
*/
int ServerSetup(int port);

/**
* Brief: Sends a file
* @param sock The socket file descriptor. Assumes the socket is already connected.
* @param fileName The name of the file to send.
* 
* Reads a file from a local location and sends it over the socket. 
* The file is sent in chunks of MAX_BUFFER_SIZE bytes.
* The first chunk is prepended with "FILE:" to indicate that it is a file.
*/
void SendFile(int sock, const char *fileName);

/**
* Brief: Sends a message
* @param sock The socket file descriptor. Assumes the socket is already connected.
* @param message The message to send.
*
* Sends a message over an open socket.
*/
void SendMessage(int sock, const char *message);

void SendOption(int sock, int selection);

/**
* Brief: Receives and prints a message
* @param sock The socket file descriptor. Assumes the socket is already connected.
*
* Receives a message from an open socket and prints it to the console. 
* The message is received in chunks of MAX_BUFFER_SIZE bytes.
* Checks if the message is a file or a message, by calling isFile().
*/
bool ReceiveMessage(int sock);

/**
 * Brief: Receives a file from the server.
 * @param sock The socket file descriptor. Assumes the socket is already connected.
 * @param fileName The name of the file to receive.
 * @param filePath The path to save the file.
 * 
 * Receives a file from the server and saves it to the given path. 
 * Should be an overload of ReceiveMessage() that takes a file name and path. But, C :(
 */
bool ReceiveNamedFile(int sock, const char *fileName, const char *filePath);

/**
 * Brief: Receives a file from the server.
 * @param sock The socket file descriptor. Assumes the socket is already connected.
 * @param fileName The name of the file to receive.
 * 
 * Receives a file from the server and saves it to the current directory.
 * 
 */
bool ReceiveString(int sock, char* buffer);

/**
* Brief: Prints the local IP address
*
* Prints the local IP address of the machine running the program.
*/
void PrintLocalIP();

/**
* Brief: Listens for connections
* @param servSock The server socket file descriptor.
* @param clientIP The buffer to store the client's IP address.
* @param username The buffer to store the client's username.
* @return The client socket file descriptor.
*
* Listens for connections on the server socket 
* and stores the client's IP address in the clientIP buffer.
* Will only accept one connection per fucntion call. 
*/
int ListenForConnections(int servSock, char *clientIP, char *username);

/**
* Brief: Receives a file, saves it, and prints it to the terminal.
* @param buffer The buffer containing the file contents
* @param bufferSize The size of the buffer.
* @param fileName The name of the file to save.
*
*/
void ProcessFile(const char *buffer, ssize_t bufferSize, const char *fileName);

/**
* Brief: Checks if a buffer contains a file.
* @param buffer The buffer to check.
* @return True if the buffer contains a file, false otherwise.
*
* Checks if the buffer starts with "FILE:" to determine if it contains a file.
*/
bool IsFile(const char *buffer);

/**
* Brief: Lists the files in a directory 
*        and stores them in a fileInfo struct.
* @param path The path to the directory.
* @param count Buffer to store the number of files in the directory.
* @return A pointer to the fileInfo struct containing the file information.
*
* Lists the files in a directory and stores them in a fileInfo struct.
* The fileInfo struct contains the file name and file size.
*/
fileInfo *listFiles(const char *path, int *count);

/**
 * Brief: Checks if a user exists in the user list.
 * @param username The username to check.
 * @param users The list of users.
 * @param userCount The number of users in the list.
 * @param userExists Buffer to store the result.
 * @return index of the user in the list or the number of users if the user does not exist.
 * 
 * Checks if a user exists in the user list.
 */
int checkUser(const char *username, userInfo *users, int userCount, bool *userExists);

/**
 * Brief: checks if a directory exists and creates it if it does not.
 * @param path The path to the directory.
 * @return 0 if the directory exists or is created, -1 otherwise.
 * 
 * Checks if a directory exists and creates it if it does not.
*/
int CreateDirectoryIfNotExist(const char* path);

#endif // HELPERFUNCTIONS_H_