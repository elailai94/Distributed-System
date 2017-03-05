#include "execute_failure_message.h"

using namespace std;

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
  return 1;
}

// See interface (header file).
int ExecuteFailureMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
