#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "message.h"
#include "segment.h"
#include "execute_failure_message.h"
#include "execute_request_message.h"
#include "execute_success_message.h"
#include "loc_failure_message.h"
#include "loc_request_message.h"
#include "loc_success_message.h"
#include "register_failure_message.h"
#include "register_request_message.h"
#include "register_success_message.h"
#include "terminate_message.h"

using namespace std;

// Returns the server address
string getServerAddress() {
   char hostname[MAXHOSTNAMELEN] = {0};
   gethostname(hostname, sizeof(hostname));
   return string(hostname);
} // getServerAddress

// Returns the server port
int getServerPort(int welcomeSocket) {
   struct sockaddr_in serverAddress;
   socklen_t serverAddressLength = sizeof(serverAddress);

   int result = getsockname(welcomeSocket, (struct sockaddr*) &serverAddress,
      &serverAddressLength);
   if (result < 0) {
      exit(-1);
   } // if

   return ntohs(serverAddress.sin_port);
} // getServerPort

// Sets up the welcome socket
int setUpWelcomeSocket() {
   struct addrinfo serverAddressHints;
   struct addrinfo* serverAddressResults;

   // Creates the welcome socket
   int welcomeSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (welcomeSocket < 0) {
      exit(-1);
   } // if

   // Sets up the server address hints and results to get the server's
   // IP address and the next available port
   memset(&serverAddressHints, 0, sizeof(serverAddressHints));
   serverAddressHints.ai_family = AF_INET;
   serverAddressHints.ai_socktype = SOCK_STREAM;
   serverAddressHints.ai_flags = AI_PASSIVE;

   // Gets the server's IP address and the next available port
   int result = getaddrinfo(NULL, "0", &serverAddressHints,
      &serverAddressResults);
   if (result != 0) {
      exit(-1);
   } // if

   // Binds the socket to the server's IP address and the next
   // available port
   result = bind(welcomeSocket, serverAddressResults->ai_addr,
      serverAddressResults->ai_addrlen);
   if (result < 0) {
      exit(-1);
   } // if

   // Frees up memory allocated for the server address results
   freeaddrinfo(serverAddressResults);

   // Listens for TCP connection requests from the client
   result = listen(welcomeSocket, SOMAXCONN);
   if (result < 0) {
      exit(-1);
   } // if

   // Prints the server address and the server port on the server's
   // standard output
   cout << "SERVER_ADDRESS " << getServerAddress() << endl;
   cout << "SERVER_PORT " << getServerPort(welcomeSocket) << endl;

   return welcomeSocket;
} // setUpWelcomeSocket

int main() {
  // Sets up the welcome socket, adds it to the all sockets set and
  // sets it as the maximum socket so far
  int welcomeSocket = setUpWelcomeSocket();

  while (true) {
    struct sockaddr clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);

    // Creates the connection socket when a connection is made
    // to the welcome socket
    int connectionSocket = accept(welcomeSocket, &clientAddress,
      &clientAddressLength);
      if (connectionSocket < 0) {
        continue;
      } // if

    Segment *parsedSegment = 0;
    int result = 0;
    result = Segment::receive(connectionSocket, parsedSegment);
    if (result < 0) {
      // Closes the connection socket and removes it from the
      // all sockets set
      close(connectionSocket);
    } // if

    Message *msg = parsedSegment->getMessage();
    switch (parsedSegment->getType()) {
      case MSG_TYPE_EXECUTE_FAILURE: {
        ExecuteFailureMessage *efm = dynamic_cast<ExecuteFailureMessage *>(msg);
        cout << "Received Execute Failure Message!" << endl;
        cout << "Reason Code: " << efm->getReasonCode() << endl;
        break;
      }

      case MSG_TYPE_EXECUTE_REQUEST: {
        ExecuteRequestMessage *erm = dynamic_cast<ExecuteRequestMessage *>(msg);
        cout << "Received Execute Request Message!" << endl;
        cout << "Name: " << erm->getName() << endl;
        
        cout << "Args:  " << erm->getArgs() << endl;
        
        break;
      }

      case MSG_TYPE_EXECUTE_SUCCESS: {
        ExecuteSuccessMessage *esm = dynamic_cast<ExecuteSuccessMessage *>(msg);
        cout << "Received Execute Success Message!" << endl;
        cout << "Name: " << esm->getName() << endl;
        break;
      }

      case MSG_TYPE_LOC_FAILURE: {
        LocFailureMessage *lfm = dynamic_cast<LocFailureMessage *>(msg);
        cout << "Received Loc Failure Message!" << endl;
        cout << "Reason Code: " << lfm->getReasonCode() << endl;
        break;
      }

      case MSG_TYPE_LOC_REQUEST: {
        LocRequestMessage *lrm = dynamic_cast<LocRequestMessage *>(msg);
        cout << "Received Loc Request Message!" << endl;
        cout << "Name: " << lrm->getName() << endl;
        break;
      }

      case MSG_TYPE_LOC_SUCCESS: {
        LocSuccessMessage *lsm = dynamic_cast<LocSuccessMessage *>(msg);
        cout << "Received Loc Success Message!" << endl;
        cout << "Server Identifier: " << lsm->getServerIdentifier() << endl;
        cout << "Port: " << lsm->getPort() << endl;
        break;
      }

      case MSG_TYPE_REGISTER_FAILURE: {
        RegisterFailureMessage *rfm = dynamic_cast<RegisterFailureMessage *>(msg);
        cout << "Received Register Failure Message!" << endl;
        cout << "Reason Code: " << rfm->getReasonCode() << endl;
        break;
      }

      case MSG_TYPE_REGISTER_REQUEST: {
        RegisterRequestMessage *rrm = dynamic_cast<RegisterRequestMessage *>(msg);
        cout << "Received Register Request Message!" << endl;
        cout << "Server Identifier: " << rrm->getServerIdentifier() << endl;
        cout << "Port: " << rrm->getPort() << endl;
        cout << "Name: " << rrm->getName() << endl;
        break;
      }

      case MSG_TYPE_REGISTER_SUCCESS: {
        RegisterSuccessMessage *rsm = dynamic_cast<RegisterSuccessMessage *>(msg);
        cout << "Received Register Success Message!" << endl;
        cout << "Reason Code: " << rsm->getReasonCode() << endl;
        break;
      }

      case MSG_TYPE_TERMINATE: {
        TerminateMessage *tm = dynamic_cast<TerminateMessage *>(msg);
        cout << "Received Terminate Message!" << endl;
        break;
      }
    }
   } // while

   // Closes the welcome socket
   close(welcomeSocket);
} // main
