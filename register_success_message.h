#ifndef __REGISTER_SUCCESS_MESSAGE_H__
#define __REGISTER_SUCCESS_MESSAGE_H__

#include "message.h"

// Object definition
class RegisterSuccessMessage: public Message {
  int reasonCode;

public:
  RegisterSuccessMessage(int reasonCode);  // Constructor
  ~RegisterSuccessMessage(); // Destructor

  // Returns an integer indicating a specific warning condition
  int getReasonCode() const;

  // Calculates the length of the message
  virtual unsigned int getLength() const;

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
  virtual int send(int dataTransferSocket);

  /*
   * Creates a message to receive data and reads into it from the data
   * transfer socket
   */
  static int receive(int dataTransferSocket, Message *&parsedMessage,
    unsigned int length);
};

#endif
