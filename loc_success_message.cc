#include <cstring>
#include <sys/socket.h>

#include "loc_success_message.h"

using namespace std;

// See interface (header file).
LocSuccessMessage::LocSuccessMessage(string serverIdentifier,
  unsigned int port)
  : Message(), serverIdentifier(serverIdentifier), port(port) {} //  Constructor

// See interface (header file).
LocSuccessMessage::~LocSuccessMessage() {} // Destructor

// See interface (header file).
string LocSuccessMessage::getServerIdentifier() const {
  return serverIdentifier;
}

// See interface (header file).
unsigned int LocSuccessMessage::getPort() const {
  return port;
}

// See interface (header file).
unsigned int LocSuccessMessage::getLength() const {
  return MAX_LENGTH_SERVER_IDENTIFIER + MAX_LENGTH_PORT;
}

// See interface (header file).
int LocSuccessMessage::send(int dataTransferSocket) {
  char messageBuffer[getLength()];
  char *messageBufferPointer = messageBuffer;

  // Writes the server identifier to the buffer
  memcpy(messageBufferPointer, serverIdentifier.c_str(),
    serverIdentifier.length());
  messageBufferPointer += MAX_LENGTH_SERVER_IDENTIFIER;

  // Writes the port to the buffer
  memcpy(messageBufferPointer, &port, MAX_LENGTH_PORT);

  // Writes the message from the buffer out to the data transfer socket
  unsigned int totalNumOfBytesMessage = getLength();
  unsigned int numOfBytesLeft = totalNumOfBytesMessage;
  unsigned int totalNumOfBytesSent = 0;

  while (totalNumOfBytesSent < totalNumOfBytesMessage) {
    int numOfBytesSent =
      ::send(dataTransferSocket, messageBuffer + totalNumOfBytesSent,
        numOfBytesLeft, 0);
    if (numOfBytesSent < 0) {
      return numOfBytesSent;
    }

    totalNumOfBytesSent += numOfBytesSent;
    numOfBytesLeft += numOfBytesSent;
  }

  return totalNumOfBytesSent;
}

// See interface (header file).
int LocSuccessMessage::receive(int dataTransferSocket,
  Message *&parsedMessage, unsigned int length) {
  // Reads the message into a buffer from the data transfer socket
  char messageBuffer[length];
  unsigned int totalNumOfBytesMessage = length;
  unsigned int numOfBytesLeft = totalNumOfBytesMessage;
  unsigned int totalNumOfBytesReceived = 0;

  while (totalNumOfBytesReceived < totalNumOfBytesMessage) {
    int numOfBytesReceived =
      ::recv(dataTransferSocket, messageBuffer + totalNumOfBytesReceived,
        numOfBytesLeft, 0);
    if (numOfBytesReceived < 0 || numOfBytesReceived == 0) {
      return numOfBytesReceived;
    }

    totalNumOfBytesReceived += numOfBytesReceived;
    numOfBytesLeft -= numOfBytesReceived;
  }

  // Parses the server identifier from the buffer
  char *messageBufferPointer = messageBuffer;
  char serverIdentifierBuffer[MAX_LENGTH_SERVER_IDENTIFIER + 1] = {'\0'};
  memcpy(serverIdentifierBuffer, messageBufferPointer,
    MAX_LENGTH_SERVER_IDENTIFIER);
  string serverIdentifier = string(serverIdentifierBuffer);
  messageBufferPointer += MAX_LENGTH_SERVER_IDENTIFIER;

  // Parses the port from the buffer
  char portBuffer[MAX_LENGTH_PORT] = {'\0'};
  memcpy(portBuffer, messageBufferPointer, MAX_LENGTH_PORT);
  unsigned int port = *((unsigned int *) portBuffer);

  parsedMessage = new LocSuccessMessage(serverIdentifier, port);
  return totalNumOfBytesReceived;
}
