#include "register_failure_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
RegisterFailureMessage::RegisterFailureMessage()
  : Message() {} //  Constructor

// See interface (header file).
RegisterFailureMessage::~RegisterFailureMessage() {} // Destructor

// See interface (header file).
int RegisterFailureMessage::getReasonCode() const {
  return reasonCode;
}
