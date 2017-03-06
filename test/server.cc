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
   fd_set allSockets;
   fd_set readSockets;

   // Clears all entries from the all sockets set and the read
   // sockets set
   FD_ZERO(&allSockets);
   FD_ZERO(&readSockets);
   
   // Sets up the welcome socket, adds it to the all sockets set and
   // sets it as the maximum socket so far
   int welcomeSocket = setUpWelcomeSocket();
   FD_SET(welcomeSocket, &allSockets);
   int maxSocket = welcomeSocket;

   while (true) {
      readSockets = allSockets;

      // Checks if some of the sockets are ready to be read from
      int result = select(maxSocket + 1, &readSockets, 0, 0, 0);
      if (result < 0) {
         continue;
      } // if

      for (int i = 0; i <= maxSocket; ++i) {
         if (!FD_ISSET(i, &readSockets)) {
            continue;
         } // if
            
         if (i == welcomeSocket) {
            
            struct sockaddr clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);

            // Creates the connection socket when a connection is made
            // to the welcome socket
            int connectionSocket = accept(welcomeSocket, &clientAddress,
               &clientAddressLength);
            if (connectionSocket < 0) {
               continue;
            } // if

            // Adds the connection socket to the all sockets set
            FD_SET(connectionSocket, &allSockets);

            // Sets the connection socket as the maximum socket so far
            // if necessary
            if (connectionSocket > maxSocket) {
               maxSocket = connectionSocket;
            } // if

         } else {

            Segment *parsedSegment = 0;
            result = 0;
            result = Segment::receive(i, parsedSegment);
            if (result < 0) {
               // Closes the connection socket and removes it from the
               // all sockets set
               close(i);
               FD_CLR(i, &allSockets);
               continue;
            } // if

            RegisterRequestMessage *msg = dynamic_cast<RegisterRequestMessage *>(parsedSegment->getMessage());

            cout << msg->getLength() << endl;
            //cout << msg->getPort() << endl;

         } // if
      } // for
   } // while

   // Closes the welcome socket
   close(welcomeSocket);
} // main