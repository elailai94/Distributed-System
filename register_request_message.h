#ifndef __REGISTER_REQUEST_MESSAGE_H__
#define __REGISTER_REQUEST_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class RegisterRequestMessage: public Message {
  std::string serverIdentifier;
  unsigned int port;
  std::string name;
  int *argTypes;

  // Returns a count of the number of argument types
  unsigned int countNumOfArgTypes(int *argTypes);

  // Writes an integer out to the data transfer socket
  int sendInteger(int dataTransferSocket, unsigned int num);

  // Writes a string out to the data transfer socket
  int sendString(int dataTransferSocket, std::string text);

public:
  RegisterRequestMessage(std::string serverIdentifier, unsigned int port,
    std::string name, int *argTypes);  // Constructor
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

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
   virtual int send(int dataTransferSocket);
};

#endif
