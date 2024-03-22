#include "helperFunctions.h"

int ClientConnect(const char *serverIP, int port) {
    // Convert port from int to string
    char serverPortStr[6]; // Enough to hold all port numbers (up to 65535)
    snprintf(serverPortStr, sizeof(serverPortStr), "%d", port);

    // Setup TCP client socket with the server IP and port number as a string
    int sock = SetupTCPClientSocket(serverIP, serverPortStr);
    if (sock < 0) {
        DieWithSystemMessage("SetupTCPClientSocket() failed"); // Error if socket setup fails
    }
    return sock;
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

void SendMessage(int sock, const char *message) {
    ssize_t numBytes = send(sock, message, strlen(message), 0);
    printf("Bytes sent: %ld\n", numBytes); // Print the number of bytes sent
    if (numBytes < 0) {
        DieWithSystemMessage("send() failed"); // Error if send fails
    } else if (numBytes != strlen(message)) {
        DieWithUserMessage("send()", "sent unexpected number of bytes"); // Error if unexpected number of bytes sent
    }
}


void SendFile(int sock, const char *fileName) {
    FILE *file = fopen(fileName, "rb"); // Open the file for reading in binary mode
    if (file == NULL) {
        DieWithUserMessage("File open failed", fileName); // Error if file can't be opened
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
}

bool ReceiveMessage(int sock) {
    char recvBuffer[MAX_BUFFER_SIZE]; // Buffer for the thank you message

    // Attempt to receive message
    ssize_t numBytesRcvd = recv(sock, recvBuffer, MAX_BUFFER_SIZE - 1, 0);

    // Check if the receive was successful
    if (numBytesRcvd < 0) {
        DieWithSystemMessage("recv() failed"); // Error if receive fails
        return false; // Return false if there's an error
    } else if (numBytesRcvd == 0) {
        // Connection closed by the server
        return false; // No data received, return false
    }

    if(IsFile(recvBuffer)) {
        ProcessFile(recvBuffer, numBytesRcvd, "receivedFile.txt");
        return true;
    }
    else{
        recvBuffer[numBytesRcvd] = '\0'; // Terminate the string
        printf("Received: %s\n", recvBuffer); // Print the thank you message
        return true; // Successfully received a thank you note, return true
    }
}

int ListenForConnections(int servSock, char *clientIP) {
    struct sockaddr_in clntAddr; // Client address structure
    socklen_t clntAddrLen = sizeof(clntAddr); // Length of the client address structure

    // Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
    if (clntSock < 0) {
        DieWithSystemMessage("accept() failed");
    }

	printf("Handling client %s\n", inet_ntoa(clntAddr.sin_addr));

    // Convert the client's IP address to a string
    //char senderIP[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &clntAddr.sin_addr, clientIP, INET_ADDRSTRLEN) == NULL) {
        DieWithSystemMessage("Error getting sender IP address");
    }

    printf("Connection established on socket %d\n", clntSock); // Print the client socket
    return clntSock;
}

bool IDUser(int sock, const char *username) {
    char message[MAX_BUFFER_SIZE];
    snprintf(message, sizeof(message), "ID:%s", username);
    SendMessage(sock, message);
    return ReceiveMessage(sock);
}

void ProcessMessage(const char *buffer, ssize_t bufferSize) {
    if (IsFile(buffer)) {
        ProcessFile(buffer, bufferSize, "receivedFile.txt");
    } else {
        printf("Received: %s\n", buffer);
    }
}

bool IsFile(const char *buffer) {
    // Check if buffer starts with "FILE:" to determine if it contains a file
    return strncmp(buffer, "FILE:", 5) == 0;
}

void ProcessFile(const char *buffer, ssize_t bufferSize, const char *fileName) {
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