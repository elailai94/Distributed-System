#ifndef __TERMINATE_MESSAGE_H__
#define __TERMINATE_MESSAGE_H__

#include "message.h"

// Object definition
class TerminateMessage: public Message {
public:
  TerminateMessage();  // Constructor
  ~TerminateMessage(); // Destructor
};

#endif
