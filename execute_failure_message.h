#ifndef __EXECUTE_FAILURE_MESSAGE_H__
#define __EXECUTE_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class ExecuteFailureMessage: public Message {
  int reasonCode;

public:
  ExecuteFailureMessage();  // Constructor
  ~ExecuteFailureMessage(); // Destructor

  // Returns an integer indicating a specific failure condition
  int getReasonCode() const;
};

#endif
