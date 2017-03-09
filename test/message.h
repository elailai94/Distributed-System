#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <sys/param.h>

#define MAX_LENGTH_SERVER_IDENTIFIER      MAXHOSTNAMELEN
#define MAX_LENGTH_PORT                   sizeof(unsigned int)
#define MAX_LENGTH_NAME                   64
#define MAX_LENGTH_ARG_TYPE               sizeof(int)
#define MAX_LENGTH_ARG                    sizeof(void *)
#define MAX_LENGTH_ARG_CHAR               sizeof(char)
#define MAX_LENGTH_ARG_SHORT              sizeof(short)
#define MAX_LENGTH_ARG_INT                sizeof(int)
#define MAX_LENGTH_ARG_LONG               sizeof(long)
#define MAX_LENGTH_ARG_DOUBLE             sizeof(double)
#define MAX_LENGTH_ARG_FLOAT              sizeof(float)
#define MAX_LENGTH_REASON_CODE            sizeof(int)

#define ARG_TYPE_IO_NATURE_MASK           0xFF000000
#define ARG_TYPE_INFORMATION_MASK         0x00FF0000
#define ARG_TYPE_ARRAY_LENGTH_MASK        0x0000FFFF
#define ARG_TYPE_INFORMATION_SHIFT_AMOUNT 16

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
