#include "loc_failure_message.h"

using namespace std;

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
int LocFailureMessage::send(int dataTransferSocket, unsigned int length) {
  return 1;
}

// See interface (header file).
int LocFailureMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
