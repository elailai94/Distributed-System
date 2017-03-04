#include "register_success_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
RegisterSuccessMessage::RegisterSuccessMessage(int reasonCode)
  : Message(), reasonCode(reasonCode) {} //  Constructor

// See interface (header file).
RegisterSuccessMessage::~RegisterSuccessMessage() {} // Destructor

// See interface (header file).
int RegisterSuccessMessage::getReasonCode() const {
  return reasonCode;
}

// See interface (header file).
int RegisterSuccessMessage::send(int dataTransferSocket, unsigned int length) {
  return 1;
}

// See interface (header file).
int RegisterSuccessMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
