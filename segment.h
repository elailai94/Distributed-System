#ifndef __SEGMENT_H__
#define __SEGMENT_H__

/* Message codes */
//Basic
#define SUCCESS = 0
#define FAILURE = -1
#define SOCKET_SEND_FAILED = -2
#define SOCKET_RECV_FAILED = -3
#define SOCKET_BAD_HOST = -4

//Client
#define FUNCTION_NOT_EXIST = -5
#define BINDER_NOT_EXIST = -6

//Server
#define FUNCTION_OVERRIDDEN = 2
#define EXECUTE_SKELETON_UNKNOWN = -7

//Binder
#define RECEIVE_INVALID_NAME =  -8
#define RECEIVE_INVALID_ARGTYPE = -9
#define RECEIVE_INVALID_PORT =  -10
#define RECEIVE_INVALID_SERVERADDRESS = -11
#define BINDER_ADDRESS_NOT_EXIST = -12
#define BINDER_PORT_NOT_EXIST = -13
#define BINDER_SOCKET_NOT_EXIST = -14

// Object declaration
class Message;

// Object definition
class Segment {
  unsigned int length;
  unsigned int type;
  Message *message;

public:
  Segment(unsigned int length, unsigned int type,
    Message *message); // Constructor
  ~Segment();          // Destructor

  // Returns the length of the message
  unsigned int getLength() const;

  // Returns the type of the message
  unsigned int getType() const;

  // Returns the message
  Message *getMessage() const;

  /*
   * Creates a segment to send data and writes it out to the data
   * transfer socket
   */
  int send(int dataTransferSocket);

  /*
   * Creates a segment to receive data and reads into it from the data
   * transfer socket
   */
  static int receive(int dataTransferSocket, Segment *&parsedSegment);
};

#endif
