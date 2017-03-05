#include "loc_request_message.h"

using namespace std;

// See interface (header file).
unsigned int LocRequestMessage::countNumOfArgTypes(int *argTypes) {
  int count = 1;
  while (argTypes[(count - 1)] != 0) {
    count += 1;
  }

  return count;
}

// See interface (header file).
LocRequestMessage::LocRequestMessage(string name, int* argTypes)
  : Message(), name(name), argTypes(argTypes) {} //  Constructor

// See interface (header file).
LocRequestMessage::~LocRequestMessage() {} // Destructor

// See interface (header file).
string LocRequestMessage::getName() const {
  return name;
}

// See interface (header file).
int *LocRequestMessage::getArgTypes() const {
  return argTypes;
}

// See interface (header file).
unsigned int LocRequestMessage::getLength() const {
  unsigned int numOfArgTypes = countNumOfArgTypes(argTypes);
  return MAX_LENGTH_NAME + (numOfArgTypes * MAX_LENGTH_ARG_TYPE);
}

// See interface (header file).
int LocRequestMessage::send(int dataTransferSocket) {
  return 1;
}

// See interface (header file).
int LocRequestMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
