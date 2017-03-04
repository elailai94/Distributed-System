#include "loc_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
LocRequestMessage::LocRequestMessage(char* name, int* argTypes)
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
int LocRequestMessage::send(int dataTransferSocket) {
  return 1;
}

// See interface (header file).
int LocRequestMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
