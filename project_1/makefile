# Makefile for compiling the TCP Token Ring program

# Compiler settings - Can change these according to your environment
CC = gcc
CFLAGS = -std=gnu99
RM = rm -f

# Source files
SOURCES = TCPTokenRing.c AddressUtility.c TCPServerUtility.c TCPClientUtility.c DieWithMessage.c

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
EXECUTABLE = ring

all: $(EXECUTABLE) clean

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(OBJECTS)

clean:
	$(RM) *.o

# Dependency rules for the object files
TCPTokenRing.o: TCPTokenRing.c
AddressUtility.o: AddressUtility.c
TCPServerUtility.o: TCPServerUtility.c
TCPClientUtility.o: TCPClientUtility.c
DieWithMessage.o: DieWithMessage.c
