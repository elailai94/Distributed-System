#ifndef __EXECUTE_REQUEST_MESSAGE_H__
#define __EXECUTE_REQUEST_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class ExecuteRequestMessage: public Message {
  std::string name;
  int* argTypes;

public:
  ExecuteRequestMessage();  // Constructor
  ~ExecuteRequestMessage(); // Destructor

  // Returns the name of the remote procedure to be executed
  std::string getName() const;

  // Returns the types of the arguments
  int *getArgTypes() const;
};

#endif
