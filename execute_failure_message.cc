#include "execute_failure_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
ExecuteFailureMessage::ExecuteFailureMessage()
  : Message(_MSG_TYPE_EXECUTE_FAILURE) {} //  Constructor

// See interface (header file).
ExecuteFailureMessage::~ExecuteFailureMessage() {} // Destructor
