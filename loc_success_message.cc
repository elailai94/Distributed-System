#include "loc_success_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
LocSuccessMessage::LocSuccessMessage(string serverIdentifier,
  unsigned int port) : Message(_MSG_TYPE_LOC_SUCCESS) {} //  Constructor

// See interface (header file).
LocSuccessMessage::~LocSuccessMessage() {} // Destructor

// See interface (header file).
string LocSuccessMessage::getServerIdentifier() const {
  return serverIdentifier;
}

// See interface (header file).
unsigned int LocSuccessMessage::getPort() const {
  return port;
}
