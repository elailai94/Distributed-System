#include <cstring>
#include <sys/socket.h>

#include "execute_failure_message.h"
#include "helper_functions.h"

// See interface (header file).
ExecuteFailureMessage::ExecuteFailureMessage(int reasonCode)
  : Message(), reasonCode(reasonCode) {} //  Constructor

// See interface (header file).
ExecuteFailureMessage::~ExecuteFailureMessage() {} // Destructor

// See interface (header file).
int ExecuteFailureMessage::getReasonCode() const {
  return reasonCode;
}

// See interface (header file).
unsigned int ExecuteFailureMessage::getLength() const {
  return MAX_LENGTH_REASON_CODE;
}

// See interface (header file).
int ExecuteFailureMessage::send(int dataTransferSocket) {
  char messageBuffer[getLength()] = {'\0'};
  char *messageBufferPointer = messageBuffer;

  // Writes the reason code to the buffer
  memcpy(messageBufferPointer, &reasonCode, MAX_LENGTH_REASON_CODE);

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
int ExecuteFailureMessage::receive(int dataTransferSocket,
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

  // Parses the reasonCode from the buffer
  char *messageBufferPointer = messageBuffer;
  char reasonCodeBuffer[MAX_LENGTH_REASON_CODE] = {'\0'};
  memcpy(reasonCodeBuffer, messageBufferPointer, MAX_LENGTH_REASON_CODE);
  int reasonCode = toInt(reasonCodeBuffer);

  parsedMessage = new ExecuteFailureMessage(reasonCode);
  return totalNumOfBytesReceived;
}
