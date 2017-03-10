#include <cstring>
#include <vector>
#include <sys/socket.h>

#include "execute_success_message.h"
#include "helper_functions.h"
#include "rpc.h"

using namespace std;

// See interface (header file).
ExecuteSuccessMessage::ExecuteSuccessMessage(string name, int *argTypes,
  void **args)
  : Message(), name(name), argTypes(argTypes), args(args) {} //  Constructor

// See interface (header file).
ExecuteSuccessMessage::~ExecuteSuccessMessage() {} // Destructor

// See interface (header file).
string ExecuteSuccessMessage::getName() const {
  return name;
}

// See interface (header file).
int *ExecuteSuccessMessage::getArgTypes() const {
  return argTypes;
}

// See interface (header file).
void **ExecuteSuccessMessage::getArgs() const {
  return args;
}

// See interface (header file).
unsigned int ExecuteSuccessMessage::getArgsLength() const {
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  unsigned int numOfArgs = numOfArgTypes - 1;
  unsigned int argsLength = 0;

  for (unsigned int i = 0; i < numOfArgs; i++) {
    int argType = argTypes[i];
    int argTypeInformation =
      (argType & ARG_TYPE_INFORMATION_MASK) >> ARG_TYPE_INFORMATION_SHIFT_AMOUNT;
    int argTypeArrayLength = argType & ARG_TYPE_ARRAY_LENGTH_MASK;
    argTypeArrayLength = (argTypeArrayLength == 0) ? 1: argTypeArrayLength;

    switch (argTypeInformation) {
      case ARG_CHAR: {
        argsLength += argTypeArrayLength * MAX_LENGTH_ARG_CHAR;
        break;
      }

      case ARG_SHORT: {
        argsLength += argTypeArrayLength * MAX_LENGTH_ARG_SHORT;
        break;
      }

      case ARG_INT: {
        argsLength += argTypeArrayLength * MAX_LENGTH_ARG_INT;
        break;
      }

      case ARG_LONG: {
        argsLength += argTypeArrayLength * MAX_LENGTH_ARG_LONG;
        break;
      }

      case ARG_DOUBLE: {
        argsLength += argTypeArrayLength * MAX_LENGTH_ARG_DOUBLE;
        break;
      }

      case ARG_FLOAT: {
        argsLength += argTypeArrayLength * MAX_LENGTH_ARG_FLOAT;
        break;
      }
    }
  }

  return argsLength;
}

// See interface (header file).
unsigned int ExecuteSuccessMessage::getLength() const {
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  return MAX_LENGTH_NAME + (numOfArgTypes * MAX_LENGTH_ARG_TYPE) +
    getArgsLength();
}

// See interface (header file).
int ExecuteSuccessMessage::send(int dataTransferSocket) {
  char messageBuffer[getLength()] = {'\0'};
  char *messageBufferPointer = messageBuffer;

  // Writes the remote procedure name to the buffer
  memcpy(messageBufferPointer, name.c_str(), name.length());
  messageBufferPointer += MAX_LENGTH_NAME;

  // Writes the argument types to the buffer
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  memcpy(messageBufferPointer, argTypes,
    numOfArgTypes * MAX_LENGTH_ARG_TYPE);
  messageBufferPointer += numOfArgTypes * MAX_LENGTH_ARG_TYPE;

  // Writes the arguments to the buffer
  unsigned int numOfArgs = numOfArgTypes - 1;
  for (unsigned int i = 0; i < numOfArgs; i++) {
    int argType = argTypes[i];
    int argTypeInformation =
      (argType & ARG_TYPE_INFORMATION_MASK) >> ARG_TYPE_INFORMATION_SHIFT_AMOUNT;
    int argTypeArrayLength = argType & ARG_TYPE_ARRAY_LENGTH_MASK;
    argTypeArrayLength = (argTypeArrayLength == 0) ? 1: argTypeArrayLength;

    switch (argTypeInformation) {
      case ARG_CHAR: {
        memcpy(messageBufferPointer, args[i],
          argTypeArrayLength * MAX_LENGTH_ARG_CHAR);
        messageBufferPointer += argTypeArrayLength * MAX_LENGTH_ARG_CHAR;
        break;
      }

      case ARG_SHORT: {
        memcpy(messageBufferPointer, args[i],
          argTypeArrayLength * MAX_LENGTH_ARG_SHORT);
        messageBufferPointer += argTypeArrayLength * MAX_LENGTH_ARG_SHORT;
        break;
      }

      case ARG_INT: {
        memcpy(messageBufferPointer, args[i],
          argTypeArrayLength * MAX_LENGTH_ARG_INT);
        messageBufferPointer += argTypeArrayLength * MAX_LENGTH_ARG_INT;
        break;
      }

      case ARG_LONG: {
        memcpy(messageBufferPointer, args[i],
          argTypeArrayLength * MAX_LENGTH_ARG_LONG);
        messageBufferPointer += argTypeArrayLength * MAX_LENGTH_ARG_LONG;
        break;
      }

      case ARG_DOUBLE: {
        memcpy(messageBufferPointer, args[i],
          argTypeArrayLength * MAX_LENGTH_ARG_DOUBLE);
        messageBufferPointer += argTypeArrayLength * MAX_LENGTH_ARG_DOUBLE;
        break;
      }

      case ARG_FLOAT: {
        memcpy(messageBufferPointer, args[i],
          argTypeArrayLength * MAX_LENGTH_ARG_FLOAT);
        messageBufferPointer += argTypeArrayLength * MAX_LENGTH_ARG_FLOAT;
        break;
      }
    }
  }

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
int ExecuteSuccessMessage::receive(int dataTransferSocket,
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
  string name(nameBuffer);
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
  unsigned int numOfArgs = argTypesBuffer.size() - 1;
  void **args = new void*[numOfArgs];
  for (unsigned int i = 0; i < numOfArgs; i++) {
    int argType = argTypes[i];
    int argTypeInformation =
      (argType & ARG_TYPE_INFORMATION_MASK) >> ARG_TYPE_INFORMATION_SHIFT_AMOUNT;
    int argTypeArrayLength = argType & ARG_TYPE_ARRAY_LENGTH_MASK;
    argTypeArrayLength = (argTypeArrayLength == 0) ? 1: argTypeArrayLength;

    switch (argTypeInformation) {
      case ARG_CHAR: {
        char *argCharArray = new char[argTypeArrayLength];
        memcpy(argCharArray, messageBufferPointer, argTypeArrayLength);
        args[i] = static_cast<void *>(argCharArray);
        messageBufferPointer += argTypeArrayLength;
        break;
      }

      case ARG_SHORT: {
        short *argShortArray = new short[argTypeArrayLength];
        for (int j = 0; j < argTypeArrayLength; j++) {
          char argShortBuffer[MAX_LENGTH_ARG_SHORT] = {'\0'};
          memcpy(argShortBuffer, messageBufferPointer, MAX_LENGTH_ARG_SHORT);
          argShortArray[j] = toShort(argShortBuffer);
          messageBufferPointer += MAX_LENGTH_ARG_SHORT;
        }
        args[i] = static_cast<void *>(argShortArray);
        break;
      }

      case ARG_INT: {
        int *argIntArray = new int[argTypeArrayLength];
        for (int j = 0; j < argTypeArrayLength; j++) {
          char argIntBuffer[MAX_LENGTH_ARG_INT] = {'\0'};
          memcpy(argIntBuffer, messageBufferPointer, MAX_LENGTH_ARG_INT);
          argIntArray[j] = toInt(argIntBuffer);
          messageBufferPointer += MAX_LENGTH_ARG_INT;
        }
        args[i] = static_cast<void *>(argIntArray);
        break;
      }

      case ARG_LONG: {
        long *argLongArray = new long[argTypeArrayLength];
        for (int j = 0; j < argTypeArrayLength; j++) {
          char argLongBuffer[MAX_LENGTH_ARG_LONG] = {'\0'};
          memcpy(argLongBuffer, messageBufferPointer, MAX_LENGTH_ARG_LONG);
          argLongArray[j] = toLong(argLongBuffer);
          messageBufferPointer += MAX_LENGTH_ARG_LONG;
        }
        args[i] = static_cast<void *>(argLongArray);
        break;
      }

      case ARG_DOUBLE: {
        double *argDoubleArray = new double[argTypeArrayLength];
        for (int j = 0; j < argTypeArrayLength; j++) {
          char argDoubleBuffer[MAX_LENGTH_ARG_DOUBLE] = {'\0'};
          memcpy(argDoubleBuffer, messageBufferPointer, MAX_LENGTH_ARG_DOUBLE);
          argDoubleArray[j] = toDouble(argDoubleBuffer);
          messageBufferPointer += MAX_LENGTH_ARG_DOUBLE;
        }
        args[i] = static_cast<void *>(argDoubleArray);
        break;
      }

      case ARG_FLOAT: {
        float *argFloatArray = new float[argTypeArrayLength];
        for (int j = 0; j < argTypeArrayLength; j++) {
          char argFloatBuffer[MAX_LENGTH_ARG_FLOAT] = {'\0'};
          memcpy(argFloatBuffer, messageBufferPointer, MAX_LENGTH_ARG_FLOAT);
          argFloatArray[j] = toFloat(argFloatBuffer);
          messageBufferPointer += MAX_LENGTH_ARG_FLOAT;
        }
        args[i] = static_cast<void *>(argFloatArray);
        break;
      }
    }
  }

  parsedMessage = new ExecuteSuccessMessage(name, argTypes, args);
  return totalNumOfBytesReceived;
}
