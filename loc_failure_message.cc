#include "loc_failure_message.h"

// See interface (header file).
LocFailureMessage::LocFailureMessage(int reasonCode)
  : Message(), reasonCode(reasonCode) {} //  Constructor

// See interface (header file).
LocFailureMessage::~LocFailureMessage() {} // Destructor

// See interface (header file).
int LocFailureMessage::getReasonCode() const {
  return reasonCode;
}

// See interface (header file).
unsigned int LocFailureMessage::getLength() const {
  return MAX_LENGTH_REASON_CODE;
}

// See interface (header file).
int LocFailureMessage::send(int dataTransferSocket) {
  return 1;
}

// See interface (header file).
int LocFailureMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
