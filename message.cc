#include <sys/socket.h>
#include "message.h"

using namespace std;

// See interface (header file).
Message::Message(unsigned int length, unsigned int type)
  : length(length), type(type) {} // Constructor

// See interface (header file).
unsigned int Message::getLength() const {
  return length;
}

// See interface (header file).
unsigned int Message::getType() const {
  return type;
}

// See interface (header file).
void Message::send(int dataTransferSocket) {
  // Sends the length of the message
  ::send(dataTransferSocket, &length, sizeof(length), 0);
  // Sends the type of the message
  ::send(dataTransferSocket, &type, sizeof(type), 0);
}

// See interface (header file).
void Message::receive(int dataTransferSocket) {
  int 
  ::recv(dataTransferSocket, &length, sizeof(length), 0)
}
