#ifndef __REGISTER_SUCCESS_MESSAGE_H__
#define __REGISTER_SUCCESS_MESSAGE_H__

#include "message.h"

// Object definition
class RegisterSuccessMessage: public Message {
  int reasonCode;

public:
  RegisterSuccessMessage();  // Constructor
  ~RegisterSuccessMessage(); // Destructor

  // Returns an integer indicating a specific warning condition
  int getReasonCode() const;
};

#endif
