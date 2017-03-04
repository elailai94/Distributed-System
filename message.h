#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define MSG_TYPE_

class Message {
public:
  Message();              // Constructor
  virtual ~Message() = 0; // Destructor

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
  virtual int send(int dataTransferSocket) = 0;

  /*
   * Creates a message to receive data and reads into it from the data
   * transfer socket
   */
  virtual void receive(int dataTransferSocket) = 0;
};

#endif
