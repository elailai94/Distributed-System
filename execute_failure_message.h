#ifndef __EXECUTE_FAILURE_MESSAGE_H__
#define __EXECUTE_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class ExecuteFailureMessage: public Message {
  int reasonCode;

public:
  ExecuteFailureMessage(int reasonCode);  // Constructor
  ~ExecuteFailureMessage(); // Destructor

  // Returns an integer indicating a specific failure condition
  int getReasonCode() const;

  // Calculates the length of the message
  virtual unsigned int getLength() const;

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
  virtual int send(int dataTransferSocket, unsigned int length);

  /*
   * Creates a message to receive data and reads into it from the data
   * transfer socket
   */
  static int receive(int dataTransferSocket, Message *parsedMessage,
    unsigned int length);
};

#endif
