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

// Sends request to the server
void sendRequest(int clientSocket) {
   // Removes a message from the messages not sent queue and
   // writes it out to the client socket
   Message messageToServer = messagesNotSent.front();
   pthread_mutex_lock(&messagesNotSentMutex);
   messagesNotSent.pop();
   pthread_mutex_unlock(&messagesNotSentMutex);
   messageToServer.send(clientSocket);

   // Creates a message to receive data from the server and reads
   // into it from the client socket
   Message messageFromServer = Message("");
   int result = 0;
   Message::receive(clientSocket, messageFromServer, result);
   if (result == 0) {
      string titleCaseText = messageFromServer.getText();
      cout << "Server: " << titleCaseText << endl;
   } // if
         
   // Delays for two seconds between successive requests
   sleep(2);
} // sendRequest


int main() {
   // Checks the number and formats of the environment variables passed
   checkEnvironmentVariables();

   // Sets up the client socket
   int clientSocket = setUpClientSocket();

   string serverIdentifier = "ubuntu1404-002.student.cs.uwaterloo.ca";
   unsigned int port = 80;
   string name = "func";
   int argTypes[3] = {1, 2, 3};
   Message msg = RegisterRequestMessage(serverIdentifier, port, name, argTypes);
   Segment seg = Segment(msg.getLength(), MSG_TYPE_EXECUTE_REQUEST, &msg);
   seg.send(clientSocket);

   // Closes the client socket
   close(clientSocket);
} // main
