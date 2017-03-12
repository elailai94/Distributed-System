#include "terminate_message.h"

// See interface (header file).
TerminateMessage::TerminateMessage()
  : Message() {} //  Constructor

// See interface (header file).
TerminateMessage::~TerminateMessage() {} // Destructor

// See interface (header file).
unsigned int TerminateMessage::getLength() const {
  return 0;
}

// See interface (header file).
int TerminateMessage::send(int dataTransferSocket) {
  return SUCCESS_CODE;
}

// See interface (header file).
int TerminateMessage::receive(int dataTransferSocket,
  Message *&parsedMessage, unsigned int length) {
  return SUCCESS_CODE;
}
