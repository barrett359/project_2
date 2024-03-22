#include "Practical.h"
#include "helperFunctions.h"

//
int main(int argc, char *argv[]) {
    if (argc != 3) {
       printf("Not enough arguments, exiting\n");
       return 0;
    }
    char *serverIP = argv[1];
    int port = atoi(argv[2]);

    int serverSocket = ClientConnect(serverIP, port);
    //printf("Server Socket: %d\n", serverSocket);
    ReceiveMessage(serverSocket);
    return 0;
}