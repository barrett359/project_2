#include "Practical.h"
#include "helperFunctions.h"

//
int main(int argc, char *argv[]) {
    ClientConnect(argv[1], atoi(argv[2]));
    ProcessMessage();
    return 0;
}