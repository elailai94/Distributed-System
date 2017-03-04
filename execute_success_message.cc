#include "execute_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
ExecuteSuccessMessage::ExecuteSuccessMessage(string name, int *argTypes,
  void **args)
  : Message(), name(name), argTypes(argTypes), args(args) {} //  Constructor

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

// See interface (header file).
int ExecuteSuccessMessage::send(int dataTransferSocket) {
  return 1;
}

// See interface (header file).
int ExecuteSuccessMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
