#include "execute_request_message.h"

using namespace std;

// See interface (header file).
ExecuteRequestMessage::ExecuteRequestMessage(string name, int *argTypes,
  void **args)
  : Message(), name(name), argTypes(argTypes), args(args) {} //  Constructor

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

// See interface (header file).
int ExecuteRequestMessage::send(int dataTransferSocket, unsigned int length) {
  return 1;
}

// See interface (header file).
int ExecuteRequestMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
