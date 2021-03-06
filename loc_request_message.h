#ifndef __LOC_REQUEST_MESSAGE_H__
#define __LOC_REQUEST_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class LocRequestMessage: public Message {
  std::string name;
  int *argTypes;

public:
  LocRequestMessage(std::string name, int *argTypes);  // Constructor
  ~LocRequestMessage(); // Destructor

  // Returns the name of the remote procedure to be executed
  std::string getName() const;

  // Returns the types of the arguments
  int *getArgTypes() const;

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
