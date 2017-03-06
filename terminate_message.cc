#include "terminate_message.h"

using namespace std;

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
  return 1;
}

// See interface (header file).
int TerminateMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
