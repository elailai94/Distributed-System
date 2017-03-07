#ifndef __LOC_SUCCESS_MESSAGE_H__
#define __LOC_SUCCESS_MESSAGE_H__

#include <string>
#include "message.h"

// Object definition
class LocSuccessMessage: public Message {
  std::string serverIdentifier;
  unsigned int port;

public:
  LocSuccessMessage(std::string serverIdentifier, unsigned int port);  // Constructor
  ~LocSuccessMessage(); // Destructor

  // Returns the server identifier
  std::string getServerIdentifier() const;

  /*
   * Returns the port on which the server is listening for client
   * requests
   */
  unsigned int getPort() const;

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
