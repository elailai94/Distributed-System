#include <netinet/in.h>
#include <sys/param.h>
#include <sys/socket.h>
#include "register_request_message.h"
#include "message_types.h"

using namespace std;


// See interface (header file).
unsigned int RegisterRequestMessage::countNumOfArgTypes(int *argTypes) {
  int count = 1;
  while (argTypes[(count - 1)] != 0) {
    count += 1;
  }

  return count;
}

// See interface (header file).
int RegisterRequestMessage::sendInteger(int dataTransferSocket,
  unsigned int num) {
  unsigned int orderedNum = htonl(num);
  int result = ::send(dataTransferSocket, &orderedNum, sizeof(unsigned int), 0);
  return result;
}

// See interface (header file).
int RegisterRequestMessage::sendString(int dataTransferSocket, string text,
  unsigned int textLength) {
  // Writes the text length out to the data transfer socket
  sendInteger(dataTransferSocket, textLength);

  char* convertedText = text.c_str();
  unsigned int totalNumOfBytesText = textLength;
  unsigned int numOfBytesLeft = totalNumOfBytesText;
  unsigned int totalNumOfBytesSent = 0;

  while (totalNumOfBytesSent < totalNumOfBytesText) {
    int numOfBytesSent =
       ::send(dataTransferSocket, convertedText + totalNumOfBytesSent,
         numOfBytesLeft, 0);
    if (numOfBytesSent < 0) {
      return numOfBytesSent;
    }

    totalNumOfBytesSent += numOfBytesSent;
    numOfBytesLeft -= numOfBytesSent;
  }

  return totalNumOfBytesSent;
}

// See interface (header file).
int RegisterRequestMessage::sendIntegerArray()

// See interface (header file).
RegisterRequestMessage::RegisterRequestMessage(string serverIdentifier,
  unsigned int port, string name, int *argTypes)
  : Message(0, _MSG_TYPE_REGISTER_REQUEST),
  serverIdentifier(serverIdentifier), port(port), name(name), argTypes(0) {
    // Makes a new copy of the argument types
    unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
    this.argTypes = new int[numOfArgTypes]();
    unsigned int argTypesLength = 4 * numOfArgTypes;
    memcpy(this.argTypes, argTypes, argTypesLength);

    // Calculates and sets the length of the message
    unsigned int typeLength = 4;
    unsigned int serverIdentifierLength = MAXHOSTNAMELEN + 1;
    unsigned int portLength = 4;
    unsigned int nameLength = 65;
    unsigned int messageLength = typeLength + serverIdentifierLength +
      portLength + nameLength + argTypesLength;
    setLength(messageLength);
} //  Constructor

// See interface (header file).
RegisterRequestMessage::~RegisterRequestMessage() {
  delete[] argTypes;
} // Destructor

// See interface (header file).
string RegisterRequestMessage::getServerIdentifier() const {
  return serverIdentifier;
}

// See interface (header file).
unsigned int RegisterRequestMessage::getPort() const {
  return port;
}

// See interface (header file).
string RegisterRequestMessage::getName() const {
  return name;
}

// See interface (header file).
int *RegisterRequestMessage::getArgTypes() const {
  return argTypes;
}

// See interface (header file).
int RegisterRequestMessage::send(int dataTransferSocket) {
  // Writes the length of the message out to the data transfer socket
  int result = sendInteger(dataTransferSocket, getLength());
  if (result < 0) {
    return result;
  }

  // Writes the type of the message out to the data transfer socket
  result = sendInteger(dataTransferSocket, getType());
  if (result < 0) {
    return result;
  }

  // Writes the server identifier out to the data transfer socket
  result = sendString(dataTransferSocket, serverIdentifier, MAXHOSTNAMELEN + 1);
  if (result < 0) {
    return result;
  }

  // Writes the server port out to the data transfer socket
  result = sendInteger(dataTransferSocket, port);
  if (result < 0) {
    return result;
  }

  // Writes the remote procedure name out to the data transfer socket
  result = sendString(dataTransferSocket, name, 65);
  if (result < 0) {
    return result;
  }

  // Writes the argument types out to the data transfer socket
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  result = sendIntegerArray(dataTransferSocket, argTypes, numOfArgTypes);
  if (result < 0) {
    return result;
  }
}
