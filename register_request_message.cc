#include "register_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
RegisterRequestMessage::RegisterRequestMessage()
  : Message(_MSG_TYPE_REGISTER_REQUEST) {} //  Constructor

// See interface (header file).
RegisterRequestMessage::~RegisterRequestMessage() {} // Destructor
