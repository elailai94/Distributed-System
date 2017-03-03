#include "loc_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
LocRequestMessage::LocRequestMessage(char* name, int* argTypes)
  : Message(_MSG_TYPE_LOC_REQUEST), name(name), argTypes(argTypes) {} //  Constructor

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
void LocRequestMessage::send(int dataTransferSocket) {
  
}

// See interface (header file).
void LocRequestMessage::receive(int dataTransferSocket) {

}
