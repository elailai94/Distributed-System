#ifndef __MESSAGE_RESULTS_H__
#define __MESSAGE_RESULTS_H__

//Basic 
#define SUCCESS = 0
#define FAILURE = -1
#define SOCKET_SEND_FAILED = -2
#define SOCKET_RECV_FAILED = -3
#define SOCKET_BAD_HOST = -4

//Client
#define FUNCTION_NOT_EXIST = -5
#define BINDER_NOT_EXIST = -6

//Server
#define FUNCTION_OVERRIDDEN = 2
#define EXECUTE_SKELETON_UNKNOWN = -7

//Binder
#define RECEIVE_INVALID_NAME =  -8
#define RECEIVE_INVALID_ARGTYPE = -9
#define RECEIVE_INVALID_PORT =  -10
#define RECEIVE_INVALID_SERVERADDRESS = -11


#endif
