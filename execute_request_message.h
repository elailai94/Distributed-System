#ifndef __EXECUTE_REQUEST_MESSAGE_H__
#define __EXECUTE_REQUEST_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class ExecuteRequestMessage: public Message {
  std::string name;
  int *argTypes;
  void **args;

  // Returns the length of the argument values
  unsigned int getArgsLength() const;

public:
  ExecuteRequestMessage(std::string name, int *argTypes, void **args);  // Constructor
  ~ExecuteRequestMessage(); // Destructor

  // Returns the name of the remote procedure to be executed
  std::string getName() const;

  // Returns the types of the arguments
  int *getArgTypes() const;

  // Returns the values of the arguments
  void **getArgs() const;

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
