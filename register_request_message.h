#ifndef __REGISTER_REQUEST_MESSAGE_H__
#define __REGISTER_REQUEST_MESSAGE_H__

#include "message.h"

// Object definition
class RegisterRequestMessage: public Message {
public:
  RegisterRequestMessage();  // Constructor
  ~RegisterRequestMessage(); // Destructor
};

#endif
