#ifndef __REGISTER_FAILURE_MESSAGE_H__
#define __REGISTER_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class RegisterFailureMessage: public Message {
  int reasonCode;

public:
  RegisterFailureMessage();  // Constructor
  ~RegisterFailureMessage(); // Destructor

  // Returns an integer indicating a specific failure condition
  int getReasonCode() const;
};

#endif
