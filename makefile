# Makefile for client and server programs

# Compiler settings
CC = gcc
CFLAGS = -std=gnu99
RM = rm -f
DB_CLEANUP = rm -f user_database.txt # Adjust if the user database is stored differently

# Source files
COMMON_SOURCES = helperFunctions.c AddressUtility.c TCPServerUtility.c TCPClientUtility.c DieWithMessage.c
CLIENT_SOURCES = client.c $(COMMON_SOURCES)
SERVER_SOURCES = server.c $(COMMON_SOURCES)

# Object files
CLIENT_OBJECTS = $(CLIENT_SOURCES:.c=.o)
SERVER_OBJECTS = $(SERVER_SOURCES:.c=.o)

# Executable names
CLIENT_EXECUTABLE = client
SERVER_EXECUTABLE = fileserver

all: $(CLIENT_EXECUTABLE) $(SERVER_EXECUTABLE) clean

$(CLIENT_EXECUTABLE): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $(CLIENT_EXECUTABLE) $(CLIENT_OBJECTS)

$(SERVER_EXECUTABLE): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $(SERVER_EXECUTABLE) $(SERVER_OBJECTS)

clean:
	$(RM) *.o

# Dependency rules for the object files
client.o: client.c
server.o: server.c
helperFunctions.o: helperFunctions.c
AddressUtility.o: AddressUtility.c
TCPServerUtility.o: TCPServerUtility.c
TCPClientUtility.o: TCPClientUtility.c
DieWithMessage.o: DieWithMessage.c

