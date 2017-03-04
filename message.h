#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <sys/param.h>

#define MAX_LENGTH_SERVER_IDENTIFIER MAXHOSTNAMELEN
#define MAX_LENGTH_PORT              sizeof(unsigned int)
#define MAX_LENGTH_NAME              64
#define MAX_LENGTH_ARG_TYPE          sizeof(int)
#define MAX_LENGTH_REASON_CODE       sizeof(int)

class Message {
public:
  Message();              // Constructor
  virtual ~Message() = 0; // Destructor

  /*
   * Creates a message to send data and writes it out to the data
   * transfer socket
   */
  virtual int send(int dataTransferSocket, unsigned int length) = 0;

  /*
   * Creates a message to receive data and reads into it from the data
   * transfer socket
   */
  static int receive(int dataTransferSocket, Message *parsedMessage,
    unsigned int length);
};

#endif
