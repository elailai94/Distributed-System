#ifndef __REGISTER_REQUEST_MESSAGE_H__
#define __REGISTER_REQUEST_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class RegisterRequestMessage: public Message {
  std::string serverIdentifier;
  unsigned int port;
  std::string name;
  int* argTypes;

public:
  RegisterRequestMessage();  // Constructor
  ~RegisterRequestMessage(); // Destructor

  // Returns the server identifier
  std::string getServerIdentifier() const;

  /*
   * Returns the port on which the server is listening for client
   * requests
   */
  unsigned int getPort() const;

  // Returns the name of the remote procedure to be registered
  std::string getName() const;

  // Returns the types of the arguments
  int *getArgTypes() const;
};

#endif
