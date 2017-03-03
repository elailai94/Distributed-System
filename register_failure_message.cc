#include "register_failure_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
RegisterFailureMessage::RegisterFailureMessage()
  : Message(_MSG_TYPE_REGISTER_FAILURE) {} //  Constructor

// See interface (header file).
RegisterFailureMessage::~RegisterFailureMessage() {} // Destructor
