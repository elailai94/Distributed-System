#ifndef __LOC_FAILURE_MESSAGE_H__
#define __LOC_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class LocFailureMessage: public Message {
  int reasonCode;

public:
  LocFailureMessage();  // Constructor
  ~LocFailureMessage(); // Destructor

  // Returns an integer indicating a specific failure condition
  int getReasonCode() const;
};

#endif
