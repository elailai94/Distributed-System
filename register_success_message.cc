#include "register_success_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
RegisterSuccessMessage::RegisterSuccessMessage()
  : Message() {} //  Constructor

// See interface (header file).
RegisterSuccessMessage::~RegisterSuccessMessage() {} // Destructor

// See interface (header file).
int RegisterSuccessMessage::getReasonCode() const {
  return reasonCode;
}
