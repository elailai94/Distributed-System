#include <cstring>
#include <vector>
#include <sys/socket.h>

#include "execute_request_message.h"

using namespace std;

// See interface (header file).
unsigned int ExecuteRequestMessage::countNumOfArgTypes(int *argTypes) const {
  int count = 1;
  while (argTypes[(count - 1)] != 0) {
    count += 1;
  }

  return count;
}

// See interface (header file).
ExecuteRequestMessage::ExecuteRequestMessage(string name, int *argTypes,
  void **args)
  : Message(), name(name), argTypes(argTypes), args(args) {} //  Constructor

// See interface (header file).
ExecuteRequestMessage::~ExecuteRequestMessage() {} // Destructor

// See interface (header file).
string ExecuteRequestMessage::getName() const {
  return name;
}

// See interface (header file).
int *ExecuteRequestMessage::getArgTypes() const {
  return argTypes;
}

// See interface (header file).
void **ExecuteRequestMessage::getArgs() const {
  return args;
}

// See interface (header file).
unsigned int ExecuteRequestMessage::getLength() const {
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  unsigned int numOfArgs = numOfArgTypes - 1;
  return MAX_LENGTH_NAME + (numOfArgTypes * MAX_LENGTH_ARG_TYPE) +
    (numOfArgs * MAX_LENGTH_ARG);
}

// See interface (header file).
int ExecuteRequestMessage::send(int dataTransferSocket) {
  char messageBuffer[getLength()];
  char *messageBufferPointer = messageBuffer;

  // Writes the remote procedure name to the buffer
  memcpy(messageBufferPointer, name.c_str(), name.length());
  messageBufferPointer += MAX_LENGTH_NAME;

  // Writes the argument types to the buffer
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  memcpy(messageBufferPointer, argTypes,
    numOfArgTypes * MAX_LENGTH_ARG_TYPE);
  messageBufferPointer += MAX_LENGTH_ARG_TYPE;

  // Writes the arguments to the buffer
  unsigned int numOfArgs = numOfArgTypes - 1;
  memcpy(messageBufferPointer, args, numOfArgs * MAX_LENGTH_ARG);

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
int ExecuteRequestMessage::receive(int dataTransferSocket,
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

  // Parses the remote procedure name from the buffer
  char *messageBufferPointer = messageBuffer;
  char nameBuffer[MAX_LENGTH_NAME + 1] = {'\0'};
  memcpy(nameBuffer, messageBufferPointer, MAX_LENGTH_NAME);
  string name = string(nameBuffer);
  messageBufferPointer += MAX_LENGTH_NAME;

  // Parses the argument types from the buffer
  vector<int> argTypesBuffer;
  while (true) {
    char argTypeBuffer[MAX_LENGTH_ARG_TYPE] = {'\0'};
    memcpy(argTypeBuffer, messageBufferPointer, MAX_LENGTH_ARG_TYPE);
    int argType = *((int *) argTypeBuffer);
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

  // Parses the argument from the buffer
  vector<void *> argsBuffer;
  while (true) {
    char argBuffer[MAX_LENGTH_ARG] = {'\0'};
    memcpy(argBuffer, messageBufferPointer, MAX_LENGTH_ARG);
    void *arg = *((void **) argBuffer);
    argsBuffer.push_back(arg);
    messageBufferPointer += MAX_LENGTH_ARG;

    if (arg == 0) {
      break;
    }
  }

  void **args = new void *[argsBuffer.size()];
  for (unsigned int i = 0; i < argsBuffer.size(); i++) {
    args[i] = argsBuffer[i];
  }

  parsedMessage = new ExecuteRequestMessage(name, argTypes, args);
  return totalNumOfBytesReceived;
}
