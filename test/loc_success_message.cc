#include "loc_success_message.h"

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
unsigned int LocSuccessMessage::getLength() const {
  return MAX_LENGTH_SERVER_IDENTIFIER + MAX_LENGTH_PORT;
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