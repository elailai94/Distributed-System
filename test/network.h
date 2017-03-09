#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <string>

// Creates a socket
int createSocket();

// Sets up a socket to listen for TCP connection requests from other hosts
int setUpToListen(int socket);

// Sets up a socket to initiate TCP connection request to another host
int setUpToConnect(int socket, std::string address, unsigned int port);

// Returns the address of a host
std::string getHostAddress();

// Returns the port number of a socket
int getSocketPort(int socket);

// Returns the address of the binder
std::string getBinderAddress();

// Returns the port number of the binder
unsigned int getBinderPort();

// Creates a new socket when a connection is made to the socket
int acceptConnection(int socket);

// Destroys a socket
int destroySocket(int socket);

#endif
