#ifndef __EXECUTE_FAILURE_MESSAGE_H__
#define __EXECUTE_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class ExecuteFailureMessage: public Message {
public:
  ExecuteFailureMessage();  // Constructor
  ~ExecuteFailureMessage(); // Destructor
};

#endif
