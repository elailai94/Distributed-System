#ifndef __MESSAGE_H__
#define __MESSAGE_H__

class Message {
public:
  Message();              // Constructor
  virtual ~Message() = 0; // Destructor

  // Calculates the length of the message
  virtual unsigned int getLength() const = 0;

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
  virtual int send(int dataTransferSocket) = 0;

  /*
   * Creates a message to receive data and reads into it from the data
   * transfer socket
   */
  static int receive(int dataTransferSocket, Message *&parsedMessage,
    unsigned int length);
};

#endif
