#ifndef __LOC_REQUEST_MESSAGE_H__
#define __LOC_REQUEST_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class LocRequestMessage: public Message {
  std::string name; // NOTE: this field shouldn't execeed 64 characters in size
  int *argTypes;

public:
  LocRequestMessage();  // Constructor
  ~LocRequestMessage(); // Destructor

  // Returns the name of the remote procedure to be executed
  std::string getName() const;

  // Returns the types of the arguments
  int *getArgTypes() const;

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
  virtual void send(int dataTransferSocket);

  /*
   * Creates a message to receive data and reads into it from the data
   * transfer socket
   */
  virtual void receive(int dataTransferSocket);
};

#endif
