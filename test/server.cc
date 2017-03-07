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
#include "register_request_message.h"

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

            if (parsedSegment != 0) {
               Message *msg = parsedSegment->getMessage();
              	  cout << "Segment type is: " << parsedSegment->getType() << endl;
		if (msg != 0) {
                  cout << "Message isn't null...." << endl;
                  RegisterRequestMessage *rrm = dynamic_cast<RegisterRequestMessage *>(msg);
                	 
		 if (rrm != 0) {
                     cout << "RRM isn't null..." << endl;
		     cout << "Server Identifier: " << rrm->getServerIdentifier() << endl;
                	
		     int tempInt = rrm->getPort(); 
		     cout << "Port: " << tempInt << endl;

                     string rrmName  = rrm->getName();
		     int strLen = rrmName.length();

		     cout << "Name: " << rrmName << endl;
                     cout << "Name Length: " << strLen << endl;
		     cout << "ArgTypes: " << *(rrm->getArgTypes()) << ", " << *(rrm->getArgTypes()+1) << ", " << *(rrm->getArgTypes()+2)  << ", " << *(rrm->getArgTypes()+3) << endl;
		}
               }
               //cout << parsedSegment->getLength() << endl;
               //cout << parsedSegment->getType() << endl;
            }
            //RegisterRequestMessage *msg = dynamic_cast<RegisterRequestMessage *>(parsedSegment->getMessage());

            //cout << msg->getLength() << endl;
            //cout << msg->getPort() << endl;
   } // while

   // Closes the welcome socket
   close(welcomeSocket);
} // main
