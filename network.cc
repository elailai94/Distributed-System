#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "network.h"
#include "constants.h"
#include "helper_functions.h"

using namespace std;

// See interface (header file).
int createSocket() {
  int socket = ::socket(AF_INET, SOCK_STREAM, 0);
  if (socket < 0) {
    return ERROR_CODE_SOCKET_CREATION_FAILED;
  }

  return socket;
}

// See interface (header file).
int setUpToListen(int socket) {
  struct addrinfo hostAddressHints;
  struct addrinfo *hostAddressResults;

  // Sets up the host address hints and results to get the host's
  // IP address and the next available port
  memset(&hostAddressHints, 0, sizeof(hostAddressHints));
  hostAddressHints.ai_family = AF_INET;
  hostAddressHints.ai_socktype = SOCK_STREAM;
  hostAddressHints.ai_flags = AI_PASSIVE;

  // Gets the host's IP address and the next available port
  int result = getaddrinfo(0, "0", &hostAddressHints,
    &hostAddressResults);
  if (result != 0) {
     return ERROR_CODE_ADDR_INFO_NOT_FOUND;
  }

  // Binds the socket to the host's IP address and the next
  // available port
  result = bind(socket, hostAddressResults->ai_addr,
    hostAddressResults->ai_addrlen);
  if (result < 0) {
    freeaddrinfo(hostAddressResults);
    return ERROR_CODE_SOCKET_BINDING_FAILED;
  }

  // Frees up memory allocated for the host address results
  freeaddrinfo(hostAddressResults);

  // Listens for TCP connection requests from other hosts
  result = listen(socket, SOMAXCONN);
  if (result < 0) {
    return ERROR_CODE_SOCKET_LISTENING_FAILED;
  }

  return SUCCESS_CODE;
}

// See interface (header file).
int setUpToConnect(int socket, string address, unsigned int port) {
  struct addrinfo hostAddressHints;
  struct addrinfo *hostAddressResults;

  // Sets up the host address hints and results to perform the DNS
  // lookup on the host's address to obtain the host's IP address
  memset(&hostAddressHints, 0, sizeof(hostAddressHints));
  hostAddressHints.ai_family = AF_INET;
  hostAddressHints.ai_socktype = SOCK_STREAM;

  // Performs a DNS lookup on the host's address to obtain the
  // host's IP address
  int result = getaddrinfo(address.c_str(), toString(port).c_str(),
    &hostAddressHints, &hostAddressResults);
  if (result != 0) {
    return ERROR_CODE_ADDR_INFO_NOT_FOUND;
  }

  // Initiates the TCP connection request to another host
  result = connect(socket, hostAddressResults->ai_addr,
    hostAddressResults->ai_addrlen);
  if (result < 0) {
    freeaddrinfo(hostAddressResults);
    return ERROR_CODE_SOCKET_CONNECTING_FAILED;
  }

  // Frees up memory allocated for the host address results
  freeaddrinfo(hostAddressResults);

  return SUCCESS_CODE;
}

// See interface (header file).
string getHostAddress() {
  char hostname[MAXHOSTNAMELEN + 1] = {'\0'};

  int result = gethostname(hostname, sizeof(hostname));
  if (result < 0) {
    return string("");
  }

  return string(hostname);
}

// See interface (header file).
int getSocketPort(int socket) {
  struct sockaddr_in hostAddress;
  socklen_t hostAddressLength = sizeof(hostAddress);

  int result = getsockname(socket, (struct sockaddr*) &hostAddress,
    &hostAddressLength);
  if (result < 0) {
    return ERROR_CODE_SOCKET_PORT_NOT_FOUND;
  }

  return ntohs(hostAddress.sin_port);
}

// See interface (header file).
string getBinderAddress() {
  char *binderAddress = getenv("BINDER_ADDRESS");
  if (binderAddress == 0) {
    return string("");
  }

  return string(binderAddress);
}

// See interface (header file).
int getBinderPort() {
  char *binderPort = getenv("BINDER_PORT");
  if (binderPort == 0) {
    return ERROR_CODE_BINDER_PORT_NOT_FOUND;
  }

  return toInt(string(binderPort));
}

// See interface (header file).
int acceptConnection(int socket) {
  struct sockaddr hostAddress;
  socklen_t hostAddressLength = sizeof(hostAddress);

  int newSocket = accept(socket, &hostAddress, &hostAddressLength);
  if (newSocket < 0) {
    return ERROR_CODE_SOCKET_ACCEPTING_FAILED;
  }

  return newSocket;
}

// See interface (header file).
int destroySocket(int socket) {
  int result = close(socket);
  if (result < 0) {
    return ERROR_CODE_SOCKET_DESTRUCTION_FAILED;
  }

  return SUCCESS_CODE;
}
