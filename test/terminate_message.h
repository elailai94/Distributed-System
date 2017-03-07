#ifndef __TERMINATE_MESSAGE_H__
#define __TERMINATE_MESSAGE_H__

#include "message.h"

// Object definition
class TerminateMessage: public Message {
public:
  TerminateMessage();  // Constructor
  ~TerminateMessage(); // Destructor

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
