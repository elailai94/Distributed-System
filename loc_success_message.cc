#include "loc_success_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
LocSuccessMessage::LocSuccessMessage(string serverIdentifier,
  unsigned int port)
  : Message(), serverIdentifier(serverIdentifier), port(port) {} //  Constructor

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

// See interface (header file).
int LocSuccessMessage::send(int dataTransferSocket) {
  return 1;
}

// See interface (header file).
int LocSuccessMessage::receive(int dataTransferSocket,
  Message *parsedMessage, unsigned int length) {
  return 1;
}
