#ifndef __MESSAGE_TYPES_H__
#define __MESSAGE_TYPES_H__

// Server/binder message types
#define _MSG_TYPE_REGISTER_REQUEST 1
#define _MSG_TYPE_REGISTER_SUCCESS 2
#define _MSG_TYPE_REGISTER_FAILURE 3

// Client/binder message types
#define _MSG_TYPE_LOC_REQUEST 4
#define _MSG_TYPE_LOC_SUCCESS 5
#define _MSG_TYPE_LOC_FAILURE 6

// Client/server message types
#define _MSG_TYPE_EXECUTE_REQUEST 7
#define _MSG_TYPE_EXECUTE_SUCCESS 8
#define _MSG_TYPE_EXECUTE_FAILURE 9

// Terminate message types
#define _MSG_TYPE_TERMINATE 10

#endif
