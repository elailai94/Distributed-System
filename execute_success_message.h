#ifndef __EXECUTE_SUCCESS_MESSAGE_H__
#define __EXECUTE_SUCCESS_MESSAGE_H__

#include "message.h"

// Object definition
class ExecuteSuccessMessage: public Message {
  std::string name;
  int *argTypes;

public:
  ExecuteSuccessMessage();  // Constructor
  ~ExecuteSuccessMessage(); // Destructor
};

#endif
