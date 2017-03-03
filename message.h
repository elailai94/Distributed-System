#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MSG_TYPE_

class Message {
  unsigned int length;
  unsigned int type;

public:
  Message(unsigned int length, unsigned int type); // Constructor
  virtual ~Message() = 0;                          // Destructor

  // Returns the length of the message
  unsigned int getLength() const;

  // Returns the type of the message
  unsigned int getType() const;

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
}

#endif
