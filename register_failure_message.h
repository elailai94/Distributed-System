#ifndef __REGISTER_FAILURE_MESSAGE_H__
#define __REGISTER_FAILURE_MESSAGE_H__

#include "message.h"

// Object definition
class RegisterFailureMessage: public Message {
  int reasonCode;

public:
  RegisterFailureMessage(int reasonCode);  // Constructor
  ~RegisterFailureMessage(); // Destructor

  // Returns an integer indicating a specific failure condition
  int getReasonCode() const;

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
