#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__

// Server/binder message types
#define MSG_TYPE_REGISTER_REQUEST 1
#define MSG_TYPE_REGISTER_SUCCESS 2
#define MSG_TYPE_REGISTER_FAILURE 3

// Client/binder message types
#define MSG_TYPE_LOC_REQUEST 4
#define MSG_TYPE_LOC_SUCCESS 5
#define MSG_TYPE_LOC_FAILURE 6

// Client/server message types
#define MSG_TYPE_EXECUTE_REQUEST 7
#define MSG_TYPE_EXECUTE_SUCCESS 8
#define MSG_TYPE_EXECUTE_FAILURE 9

// Terminate message types
#define MSG_TYPE_TERMINATE 10

#endif
