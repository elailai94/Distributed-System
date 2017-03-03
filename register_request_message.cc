#include "register_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
RegisterRequestMessage::RegisterRequestMessage()
  : Message(_MSG_TYPE_REGISTER_REQUEST) {} //  Constructor

// See interface (header file).
RegisterRequestMessage::~RegisterRequestMessage() {} // Destructor

// See interface (header file).
string RegisterRequestMessage::getServerIdentifier() const {
  return serverIdentifier;
}

// See interface (header file).
unsigned int RegisterRequestMessage::getPort() const {
  return port;
}

// See interface (header file).
string RegisterRequestMessage::getName() const {
  return name;
}

// See interface (header file).
int *RegisterRequestMessage::getArgTypes() const {
  return argTypes;
}
