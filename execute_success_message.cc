#include "execute_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
ExecuteSuccessMessage::ExecuteSuccessMessage()
  : Message(_MSG_TYPE_EXECUTE_SUCCESS) {} //  Constructor

// See interface (header file).
ExecuteSuccessMessage::~ExecuteSuccessMessage() {} // Destructor

// See interface (header file).
string ExecuteSuccessMessage::getName() const {
  return name;
}

// See interface (header file).
int *ExecuteSuccessMessage::getArgTypes() const {
  return argTypes;
}

// See interface (header file).
void **ExecuteSuccessMessage::getArgs() const {
  return args;
}
