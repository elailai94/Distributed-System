#ifndef __EXECUTE_SUCCESS_MESSAGE_H__
#define __EXECUTE_SUCCESS_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class ExecuteSuccessMessage: public Message {
  std::string name;
  int *argTypes;
  void **args;

public:
  ExecuteSuccessMessage();  // Constructor
  ~ExecuteSuccessMessage(); // Destructor

  // Returns the name of the remote procedure executed
  std::string getName() const;

  // Returns the types of the arguments
  int *getArgTypes() const;

  // Returns the values of the arguments
  void **getArgs() const;
};

#endif
