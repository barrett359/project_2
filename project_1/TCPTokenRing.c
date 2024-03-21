/*
*	James Williams
*	Assignment 1
*
*	This program is designed to simulate a token ring network. 
*	It will send a file to the next computer in the ring and then wait for a thank you note from the next computer. 
* 	If the computer is the first in the ring, it will also wait for a file from the last computer in the ring.
*/


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

#define MAX_BUFFER_SIZE 512
#define MAX_PENDING 5 // Maximum outstanding connection requests
char *fileName = NULL;

/**
 * Sends a file to a server specified by IP and port.
 * @param serverIP IP address of the server to send the file to.
 * @param port Port number of the server to send the file to.
 */
void ClientSendFile(const char *serverIP, int port);

/**
 * Sets up a TCP client socket to connect to the specified server IP and port.
 * @param clientIP IP address of the server to connect to.
 * @param port Port number of the server to connect to.
 */
void ClientSetup(const char *clientIP, int port);

/**
 * Sets up a TCP server socket to listen for incoming connections on the specified port.
 * @param port Port number on which the server will listen for connections.
 */
int ServerSetup(int port);

/**
 * Sends a thank you message to a server specified by IP and port.
 * @param serverIP IP address of the server to send the message to.
 * @param port Port number of the server to send the message to.
 * @param message The thank you message to be sent.
 */
void ClientSendThanks(const char *serverIP, int port, const char *message);

/**
 * Determines if the received buffer contains a file.
 * @param buffer The received buffer to be checked.
 * @return true if the buffer contains a file, false otherwise.
 */
bool isFile(const char *buffer);

/**
 * Processes and prints the content of the received file.
 * @param buffer The buffer containing the file content.
 * @param bufferSize The size of the buffer.
 */
void processFile(const char *buffer, ssize_t bufferSize);

/**
 * Waits for a file to be received from a client.
 * @param servSock The server socket to accept the file from.
 * @return true if the file was received successfully, false otherwise.
 */
bool waitForFile(int servSock);

/**
 * Prints the local IP address of the computer.
 */
void PrintLocalIP();

/**
 * Receives a thank you note from the server.
 * @param sock The socket to receive the thank you note from.
 * @param nextIP The IP address of the next computer in the ring.
 * @return true if the thank you note was received successfully, false otherwise.
 */
bool receiveThankYouNote(int sock, const char* nextIP);

int main(int argc, char *argv[]) {
    if (argc < 3) { // Check for minimum required arguments: program name, IP, port
        DieWithUserMessage("Parameter(s)", "<Server IP> <Server Port> [Filename]");
    }

    char *nextIP = argv[1]; //IP address of the next computer in the ring
    int port = atoi(argv[2]); // Port number used for the connection
    fileName = argc == 4 ? argv[3] : NULL; // Optional fileName for Computer A
	bool fileReceived = false;

    if (fileName) {
        // Computer A: send file to B, then wait for thank you note from B
		PrintLocalIP();
    	printf("Sending data to: %s\n", nextIP);
        ClientSendFile(nextIP, port);
		int servSock = ServerSetup(port);
		while(!fileReceived){
			fileReceived = waitForFile(servSock);
		}
		printf("Goodbye!\n");
    } else {
		fileName = "incomingFile.txt";
        // Computers B or C: receive file/thank you note, then send file/thank you note accordingly
		PrintLocalIP();
        int servSock = ServerSetup(port);
		while(!fileReceived){
			fileReceived = waitForFile(servSock);
		}
		ClientSendFile(nextIP, port);
		printf("Goodbye!\n");
    }
    return 0;
}

void ClientSetup(const char *clientIP, int port) {
    // Convert port from int to string
    char serverPortStr[6]; // Enough to hold all port numbers (up to 65535)
    snprintf(serverPortStr, sizeof(serverPortStr), "%d", port);

    // Setup TCP client socket with the server IP and port number as a string
    int sock = SetupTCPClientSocket(clientIP, serverPortStr);
    if (sock < 0) {
        DieWithSystemMessage("SetupTCPClientSocket() failed"); // Error if socket setup fails
    }
}

void ClientSendFile(const char *nextIP, int port) {
    FILE *file = fopen(fileName, "rb"); // Open the file for reading in binary mode
    if (file == NULL) {
        DieWithUserMessage("File open failed", fileName); // Error if file can't be opened
    }

    // Convert port from int to string
    char serverPortStr[6]; // Enough to hold all port numbers (up to 65535)
    snprintf(serverPortStr, sizeof(serverPortStr), "%d", port);

    // Setup TCP client socket with the server IP and port number as a string
    int sock = SetupTCPClientSocket(nextIP, serverPortStr);
    if (sock < 0) {
        DieWithSystemMessage("SetupTCPClientSocket() failed"); // Error if socket setup fails
    }

	bool isFirstChunk = true; // Flag to track the first chunk
    char fileBuffer[MAX_BUFFER_SIZE]; // Buffer for file content
    int bytesRead; // Number of bytes read from the file

    // Read from the file and send its content over the socket
    while ((bytesRead = fread(fileBuffer + (isFirstChunk ? 5 : 0), 1, MAX_BUFFER_SIZE - (isFirstChunk ? 5 : 0), file)) > 0) {
        if (isFirstChunk) {
            // Prepend "FILE:" to the first chunk
            memcpy(fileBuffer, "FILE:", 5);
            bytesRead += 5; // Adjust bytesRead to include "FILE:"
            isFirstChunk = false; // Reset the flag after the first chunk
        }
		if (send(sock, fileBuffer, bytesRead, 0) != bytesRead) {
            DieWithSystemMessage("send() failed"); // Error if send does not complete properly
        }
		printf("Sent %d bytes\n", bytesRead);
	}

    if (ferror(file)) {
        DieWithSystemMessage("fread() failed"); // Error on file read failure
    }

    fclose(file); // Close the file

	while(!receiveThankYouNote(sock, nextIP))

    close(sock); // Close the socket
}

bool receiveThankYouNote(int sock, const char* nextIP) {
    char recvBuffer[MAX_BUFFER_SIZE]; // Buffer for the thank you message

    // Attempt to receive message from the server
    ssize_t numBytesRcvd = recv(sock, recvBuffer, MAX_BUFFER_SIZE - 1, 0);

    // Check if the receive was successful
    if (numBytesRcvd < 0) {
        DieWithSystemMessage("recv() failed"); // Error if receive fails
        return false; // Return false if there's an error
    } else if (numBytesRcvd == 0) {
        // Connection closed by the server, might want to handle this case as well
        return false; // No data received, return false
    } else {
        recvBuffer[numBytesRcvd] = '\0'; // Terminate the string
        printf("Received a thank you note from: %s\n", nextIP);
        return true; // Successfully received a thank you note, return true
    }
}

int ServerSetup(int port) {
    // Convert port from int to string
    char serverPortStr[6]; // Enough to hold all port numbers (up to 65535)
    snprintf(serverPortStr, sizeof(serverPortStr), "%d", port);

    // Setup TCP server socket with the server port number as a string
    int servSock = SetupTCPServerSocket(serverPortStr);
    if (servSock < 0) {
        DieWithSystemMessage("SetupTCPServerSocket() failed"); // Error if server socket setup fails
    }
	return servSock;
}

bool waitForFile(int servSock){
	// Continuously accept connections and process received data
    for (;;){
        struct sockaddr_in clntAddr; // Client address structure
        socklen_t clntAddrLen = sizeof(clntAddr); // Length of the client address structure

        // Wait for a client to connect
        int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
        if (clntSock < 0) {
            DieWithSystemMessage("accept() failed");
        }

		printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

        // Convert the client's IP address to a string
        char senderIP[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &clntAddr.sin_addr, senderIP, INET_ADDRSTRLEN) == NULL) {
            DieWithSystemMessage("Error getting sender IP address");
        }

        char buffer[MAX_BUFFER_SIZE]; // Buffer for receiving data
        ssize_t numBytesRcvd = recv(clntSock, buffer, MAX_BUFFER_SIZE, 0); // Receive data
        if (numBytesRcvd < 0) {
            DieWithSystemMessage("recv() failed"); // Error if receive fails
        }

        if (isFile(buffer)) {
            processFile(buffer, numBytesRcvd); // Process received file
            // Call ClientSendThanks with the IP address of the server that sent the file

			send(clntSock, "Thank you for the file!", 23, 0);
			printf("Sending thank you note to %s\n", senderIP);
            //ClientSendThanks(senderIP, port, "Thank you for the file!");
			close(clntSock); // Close the client socket
			close(servSock); // Close the server socket
			return true;
        }
		close(clntSock); // Close the client socket
	}
}


bool isFile(const char *buffer) {
    // Check if buffer starts with "FILE:" to determine if it contains a file
    return strncmp(buffer, "FILE:", 5) == 0;
}

void processFile(const char *buffer, ssize_t bufferSize) {
    // Ensure the buffer is null-terminated
    char fileContent[MAX_BUFFER_SIZE + 1]; // +1 for null terminator
    memset(fileContent, 0, sizeof(fileContent)); // Initialize buffer with zeros
    memcpy(fileContent, buffer + 5, bufferSize - 5); // Skip "FILE:" prefix and copy the rest
    fileContent[bufferSize - 5] = '\0'; // Null-terminate the string

    printf("Received File Content:\n%s\n", fileContent);

	FILE *file = fopen(fileName, "w");
	if (file == NULL) {
		printf("Failed to open file for writing.\n");
		return;
	}

	fprintf(file, "%s", fileContent);
	fclose(file);
}

void PrintLocalIP() {
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs); // Get a list of all network interfaces
    tmp = addrs;

    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) { // Check for IPv4 address
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            // Exclude loopback address
            if (strcmp(inet_ntoa(pAddr->sin_addr), "127.0.0.1") != 0) {
                printf("My IP address is: %s\n", inet_ntoa(pAddr->sin_addr));
                break; // Break after printing the first non-loopback IPv4 address
            }
        }
        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs); // Free the linked list
}