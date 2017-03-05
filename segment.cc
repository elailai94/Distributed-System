#include <sys/socket.h>
#include "segment.h"
#include "message_types.h"
#include "register_request_message.h"
#include "register_success_message.h"
#include "register_failure_message.h"
#include "loc_request_message.h"
#include "loc_success_message.h"
#include "loc_failure_message.h"
#include "execute_request_message.h"
#include "execute_success_message.h"
#include "execute_failure_message.h"
#include "terminate_message.h"

// See interface (header file).
Segment::Segment(unsigned int length, unsigned int type, Message *message)
  : length(length), type(type), message(message) {} // Constructor

// See interface (header file).
Segment::~Segment() {
  delete message;
}

// See interface (header file).
unsigned int Segment::getLength() const {
  return length;
}

// See interface (header file).
unsigned int Segment::getType() const {
  return type;
}

// See interface (header file).
Message *Segment::getMessage() const {
  return message;
}

// See interface (header file).
int Segment::send(int dataTransferSocket) {
  // Writes the length of the message out to the data transfer socket
  int result = ::send(dataTransferSocket, &length, sizeof(length), 0);
  if (result < 0) {
    return result;
  }

  // Writes the type of the message out to the data transfer socket
  result = ::send(dataTransferSocket, &type, sizeof(type), 0);
  if (result < 0) {
    return result;
  }

  // Writes the message out to the data transfer socket
  result = message->send(dataTransferSocket);
  if (result < 0) {
    return result;
  }

  return length;
}

// See interface (header file).
int Segment::receive(int dataTransferSocket, Segment *parsedSegment) {
  // Reads the length of the message from the data transfer socket
  unsigned int length = 0;
  int result = ::recv(dataTransferSocket, &length, sizeof(length), 0);
  if (result < 0 || result == 0) {
    return result;
  }

  // Reads the type of the message from the data transfer socket
  unsigned int type = 0;
  result = ::recv(dataTransferSocket, &type, sizeof(type), 0);
  if (result < 0 || result == 0) {
    return result;
  }

  // Reads the message from the data transfer socket
  Message *parsedMessage = 0;
  switch (type) {
    case MSG_TYPE_REGISTER_REQUEST:
      result =
        RegisterRequestMessage::receive(dataTransferSocket, parsedMessage,
          length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_REGISTER_SUCCESS:
      result =
        RegisterSuccessMessage::receive(dataTransferSocket, parsedMessage,
          length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_REGISTER_FAILURE:
      result =
        RegisterFailureMessage::receive(dataTransferSocket, parsedMessage,
          length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_LOC_REQUEST:
      result = LocRequestMessage::receive(dataTransferSocket, parsedMessage,
        length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_LOC_SUCCESS:
      result = LocSuccessMessage::receive(dataTransferSocket, parsedMessage,
        length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_LOC_FAILURE:
      result = LocFailureMessage::receive(dataTransferSocket, parsedMessage,
        length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_EXECUTE_REQUEST:
      result =
        ExecuteRequestMessage::receive(dataTransferSocket, parsedMessage,
          length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_EXECUTE_SUCCESS:
      result =
        ExecuteRequestMessage::receive(dataTransferSocket, parsedMessage,
          length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_EXECUTE_FAILURE:
      result =
        ExecuteRequestMessage::receive(dataTransferSocket, parsedMessage,
          length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;

    case MSG_TYPE_TERMINATE:
      result = TerminateMessage::receive(dataTransferSocket, parsedMessage,
        length);
      if (result < 0 || result == 0) {
        return result;
      }
      break;
  }

  parsedSegment = new Segment(length, type, parsedMessage);
  return length;
}
