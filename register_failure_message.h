#ifndef __REGISTER_FAILURE_MESSAGE_H__
#define __REGISTER_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class RegisterFailureMessage: public Message {
public:
  RegisterFailureMessage();  // Constructor
  ~RegisterFailureMessage(); // Destructor
};

#endif
