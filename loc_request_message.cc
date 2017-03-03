#include "loc_request_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
LocRequestMessage::LocRequestMessage()
  : Message(_MSG_TYPE_LOC_REQUEST) {} //  Constructor

// See interface (header file).
LocRequestMessage::~LocRequestMessage() {} // Destructor
