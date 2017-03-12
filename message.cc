#include "message.h"
#include "constants.h"

// See interface (header file).
Message::Message() {} // Constructor

// See interface (header file).
Message::~Message() {} // Destructor

// See interface (header file).
int Message::receive(int dataTransferSocket, Message *&parsedMessage,
  unsigned int length) {
  return SUCCESS_CODE;
}
