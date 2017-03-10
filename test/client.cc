#include <iostream>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "message.h"
#include "segment.h"
#include "register_request_message.h"
#include "execute_request_message.h"
#include "execute_success_message.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bitset>

#define CHAR_ARRAY_LENGTH 100

#define ARG_CHAR    1
#define ARG_SHORT   2
#define ARG_INT     3
#define ARG_LONG    4
#define ARG_DOUBLE  5
#define ARG_FLOAT   6

#define ARG_INPUT   31
#define ARG_OUTPUT  30

using namespace std;

// Checks the number and formats of the environment variables passed
void checkEnvironmentVariables() {
   char* serverAddress = getenv("BINDER_ADDRESS");
   if (serverAddress == 0) {
      exit(-1);
   } // if

   char* serverPort = getenv("BINDER_PORT");
   if (serverPort == 0) {
      exit(-1);
   } // if
} // checkEnvironmentVariables

// Sets up the client socket
int setUpClientSocket() {
   struct addrinfo serverAddressHints;
   struct addrinfo* serverAddressResults;

   // Creates the client socket
   int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (clientSocket < 0) {
      exit(-1);
   } // if

   // Gets the environment variables passed
   char* serverAddress = getenv("BINDER_ADDRESS");
   char* serverPort = getenv("BINDER_PORT");

   // Sets up the server address hints and results to perform the DNS
   // lookup on the server's host name to obtain the server's IP address
   memset(&serverAddressHints, 0, sizeof(serverAddressHints));
   serverAddressHints.ai_family = AF_INET;
   serverAddressHints.ai_socktype = SOCK_STREAM;

   // Performs a DNS lookup on the server's host name to obtain the
   // server's IP address
   int result = getaddrinfo(serverAddress, serverPort,
      &serverAddressHints, &serverAddressResults);
   if (result != 0) {
      exit(-1);
   } // if

   // Initiates the TCP connection between the client and the server
   result = connect(clientSocket, serverAddressResults->ai_addr,
      serverAddressResults->ai_addrlen);
   if (result < 0) {
      exit(-1);
   } // if

   // Frees up memory allocated for the server address results
   freeaddrinfo(serverAddressResults);

   return clientSocket;
} //setUpSocket

int main() {
   // Checks the number and formats of the environment variables passed
   checkEnvironmentVariables();


   // Sets up the client socket
   int clientSocket = setUpClientSocket();

   string serverIdentifier = "ubuntu1404-002.student.cs.uwaterloo.ca";
   unsigned int port = 80;
   string name = "func";
   /* prepare the arguments for f0 */
   int a0 = 5;
   int b0 = 10;
   int count0 = 3;
   int return0;
   int argTypes0[count0 + 1];
   void **args0;

   argTypes0[0] = (1 << ARG_OUTPUT) | (ARG_INT << 16);
   cout << bitset<32>(argTypes0[0]) << endl;
   
   argTypes0[1] = (1 << ARG_INPUT) | (ARG_INT << 16);
   cout << bitset<32>(argTypes0[1]) << endl;
   argTypes0[2] = (1 << ARG_INPUT) | (ARG_INT << 16);
   cout << bitset<32>(argTypes0[2]) << endl;   
   argTypes0[3] = 0;
   cout << bitset<32>(argTypes0[3]) << endl;
   
   args0 = (void **)malloc(count0 * sizeof(void *));
   args0[0] = (void *)&return0;
   args0[1] = (void *)&a0;
   args0[2] = (void *)&b0;

   ExecuteRequestMessage msg = ExecuteRequestMessage(name, argTypes0, args0);
   //cout << "Server Identifier: " << msg.getServerIdentifier() << endl;
   //cout << "Port: " << msg.getPort() << endl;
   //cout << "Name: " << msg.getName() << endl;
   //cout << "ArgTypes: " << *(msg.getArgTypes()) << ", " << *(msg.getArgTypes() + 1) << ", " << *(msg.getArgTypes() + 2) << ", " << *(msg.getArgTypes() + 3) << ", "  <<endl;
   
   Segment seg = Segment(msg.getLength(), MSG_TYPE_EXECUTE_REQUEST, &msg);
   seg.send(clientSocket);

   // Closes the client socket

   sleep(2);

   close(clientSocket);
} // main
