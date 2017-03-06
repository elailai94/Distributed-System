#include "execute_success_message.h"

using namespace std;

// See interface (header file).
unsigned int ExecuteSuccessMessage::countNumOfArgTypes(int *argTypes) const {
  int count = 1;
  while (argTypes[(count - 1)] != 0) {
    count += 1;
  }

  return count;
}

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
unsigned int ExecuteSuccessMessage::getLength() const {
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  unsigned int numOfArgs = numOfArgTypes - 1;
  return MAX_LENGTH_NAME + (numOfArgTypes * MAX_LENGTH_ARG_TYPE) +
    (numOfArgs * MAX_LENGTH_ARG);
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
