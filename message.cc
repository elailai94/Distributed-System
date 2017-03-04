#include <sys/socket.h>
#include "message.h"

using namespace std;

// See interface (header file).
void Message::setLength(unsigned int length) {
  this.length = length;
}

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
