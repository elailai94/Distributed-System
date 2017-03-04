#include "execute_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
ExecuteRequestMessage::ExecuteRequestMessage()
  : Message(_MSG_TYPE_EXECUTE_REQUEST) {} //  Constructor

// See interface (header file).
ExecuteRequestMessage::~ExecuteRequestMessage() {} // Destructor

// See interface (header file).
string ExecuteRequestMessage::getName() const {
  return name;
}

// See interface (header file).
int *ExecuteRequestMessage::getArgTypes() const {
  return argTypes;
}

// See interface (header file).
void **ExecuteRequestMessage::getArgs() const {
  return args;
}
