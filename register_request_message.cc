#include <cstring>
#include <vector>
#include <sys/socket.h>

#include "register_request_message.h"
#include "constants.h"
#include "helper_functions.h"

using namespace std;

// See interface (header file).
RegisterRequestMessage::RegisterRequestMessage(string serverIdentifier,
  unsigned int port, string name, int *argTypes)
  : Message(), serverIdentifier(serverIdentifier), port(port),
  name(name), argTypes(argTypes) {
} //  Constructor

// See interface (header file).
RegisterRequestMessage::~RegisterRequestMessage() {} // Destructor

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
unsigned int RegisterRequestMessage::getLength() const {
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  return MAX_LENGTH_SERVER_IDENTIFIER + MAX_LENGTH_PORT + MAX_LENGTH_NAME +
    (numOfArgTypes * MAX_LENGTH_ARG_TYPE);
}

// See interface (header file).
int RegisterRequestMessage::send(int dataTransferSocket) {
  char messageBuffer[getLength()] = {'\0'};
  char *messageBufferPointer = messageBuffer;

  // Writes the server identifier to the buffer
  memcpy(messageBufferPointer, serverIdentifier.c_str(),
    serverIdentifier.length());
  messageBufferPointer += MAX_LENGTH_SERVER_IDENTIFIER;

  // Writes the port to the buffer
  memcpy(messageBufferPointer, &port, MAX_LENGTH_PORT);
  messageBufferPointer += MAX_LENGTH_PORT;

  // Writes the remote procedure name to the buffer
  memcpy(messageBufferPointer, name.c_str(), name.length());
  messageBufferPointer += MAX_LENGTH_NAME;

  // Writes the argument types to the buffer
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  memcpy(messageBufferPointer, argTypes,
    numOfArgTypes * MAX_LENGTH_ARG_TYPE);

  // Writes the message from the buffer out to the data transfer socket
  unsigned int totalNumOfBytesMessage = getLength();
  unsigned int numOfBytesLeft = totalNumOfBytesMessage;
  unsigned int totalNumOfBytesSent = 0;

  while (totalNumOfBytesSent < totalNumOfBytesMessage) {
    int numOfBytesSent =
      ::send(dataTransferSocket, messageBuffer + totalNumOfBytesSent,
        numOfBytesLeft, 0);
    if (numOfBytesSent < 0) {
      return ERROR_CODE_SOCKET_SENDING_FAILED;
    }

    totalNumOfBytesSent += numOfBytesSent;
    numOfBytesLeft += numOfBytesSent;
  }

  return SUCCESS_CODE;
}

// See interface (header file).
int RegisterRequestMessage::receive(int dataTransferSocket,
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
    if (numOfBytesReceived <= 0) {
      return ERROR_CODE_SOCKET_RECEIVING_FAILED;
    }

    totalNumOfBytesReceived += numOfBytesReceived;
    numOfBytesLeft -= numOfBytesReceived;
  }

  // Parses the server identifier from the buffer
  char *messageBufferPointer = messageBuffer;
  char serverIdentifierBuffer[MAX_LENGTH_SERVER_IDENTIFIER + 1] = {'\0'};
  memcpy(serverIdentifierBuffer, messageBufferPointer,
    MAX_LENGTH_SERVER_IDENTIFIER);
  string serverIdentifier(serverIdentifierBuffer);
  messageBufferPointer += MAX_LENGTH_SERVER_IDENTIFIER;

  // Parses the port from the buffer
  char portBuffer[MAX_LENGTH_PORT] = {'\0'};
  memcpy(portBuffer, messageBufferPointer, MAX_LENGTH_PORT);
  unsigned int port = toUnsignedInt(portBuffer);
  messageBufferPointer += MAX_LENGTH_PORT;

  // Parses the remote procedure name from the buffer
  char nameBuffer[MAX_LENGTH_NAME + 1] = {'\0'};
  memcpy(nameBuffer, messageBufferPointer, MAX_LENGTH_NAME);
  string name(nameBuffer);
  messageBufferPointer += MAX_LENGTH_NAME;

  // Parses the argument types from the buffer
  vector<int> argTypesBuffer;
  while (true) {
    char argTypeBuffer[MAX_LENGTH_ARG_TYPE] = {'\0'};
    memcpy(argTypeBuffer, messageBufferPointer, MAX_LENGTH_ARG_TYPE);
    int argType = toInt(argTypeBuffer);
    argTypesBuffer.push_back(argType);
    messageBufferPointer += MAX_LENGTH_ARG_TYPE;

    if (argType == 0) {
      break;
    }
  }

  int *argTypes = new int[argTypesBuffer.size()];
  for (unsigned int i = 0; i < argTypesBuffer.size(); i++) {
    argTypes[i] = argTypesBuffer[i];
  }

  parsedMessage =
    new RegisterRequestMessage(serverIdentifier, port, name, argTypes);
  return SUCCESS_CODE;
}
