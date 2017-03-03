#include "loc_failure_message.h"
#include "message_types.h"

using namespace std;

// See interface (header file).
LocFailureMessage::LocFailureMessage()
  : Message(_MSG_TYPE_LOC_FAILURE) {} //  Constructor

// See interface (header file).
LocFailureMessage::~LocFailureMessage() {} // Destructor
