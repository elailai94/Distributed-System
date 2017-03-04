#ifndef __LOC_FAILURE_MESSAGE_H__
#define __LOC_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class LocFailureMessage: public Message {
  int reasonCode;

public:
  LocFailureMessage(int reasonCode);  // Constructor
  ~LocFailureMessage(); // Destructor

  // Returns an integer indicating a specific failure condition
  int getReasonCode() const;

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
  virtual int send(int dataTransferSocket);

  /*
   * Creates a message to receive data and reads into it from the data
   * transfer socket
   */
  static int receive(int dataTransferSocket, Message *parsedMessage,
    unsigned int length);
};

#endif
