#include "execute_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
ExecuteSuccessMessage::ExecuteSuccessMessage()
  : Message(_MSG_TYPE_EXECUTE_SUCCESS) {} //  Constructor

// See interface (header file).
ExecuteSuccessMessage::~ExecuteSuccessMessage() {} // Destructor
