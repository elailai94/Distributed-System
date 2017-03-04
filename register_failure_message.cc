#include "register_failure_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
RegisterFailureMessage::RegisterFailureMessage(int reasonCode)
  : Message(), reasonCode(reasonCode) {} //  Constructor

// See interface (header file).
RegisterFailureMessage::~RegisterFailureMessage() {} // Destructor

// See interface (header file).
int RegisterFailureMessage::getReasonCode() const {
  return reasonCode;
}

// See interface (header file).
int RegisterFailureMessage::send(int dataTransferSocket) {
  return 1;
}

// See interface (header file).
int RegisterFailureMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
