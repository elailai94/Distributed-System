#include "terminate_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
TerminateMessage::TerminateMessage()
  : Message(_MSG_TYPE_TERMINATE) {} //  Constructor

// See interface (header file).
TerminateMessage::~TerminateMessage() {} // Destructor
