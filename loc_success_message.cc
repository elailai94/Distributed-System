#include "loc_success_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
LocSuccessMessage::LocSuccessMessage()
  : Message(_MSG_TYPE_LOC_SUCCESS) {} //  Constructor

// See interface (header file).
LocSuccessMessage::~LocSuccessMessage() {} // Destructor
