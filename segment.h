#ifndef __SEGMENT_H__
#define __SEGMENT_H__

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
  static int receive(int dataTransferSocket, Segment &parsedSegment);
};

#endif
