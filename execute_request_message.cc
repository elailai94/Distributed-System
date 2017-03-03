#include "execute_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
ExecuteRequestMessage::ExecuteRequestMessage()
  : Message(_MSG_TYPE_EXECUTE_REQUEST) {} //  Constructor

// See interface (header file).
ExecuteRequestMessage::~ExecuteRequestMessage() {} // Destructor
