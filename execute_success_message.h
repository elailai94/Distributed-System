#ifndef __EXECUTE_SUCCESS_MESSAGE_H__
#define __EXECUTE_SUCCESS_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class ExecuteSuccessMessage: public Message {
  std::string name;
  int *argTypes;
  void **args;

public:
  ExecuteSuccessMessage(std::string name, int *argTypes, void **args);  // Constructor
  ~ExecuteSuccessMessage(); // Destructor

  // Returns the name of the remote procedure executed
  std::string getName() const;

  // Returns the types of the arguments
  int *getArgTypes() const;

  // Returns the values of the arguments
  void **getArgs() const;

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
