#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <string>
#include <sys/param.h>

/************************ Message types ************************/
/* Server/binder message types */
#define MSG_TYPE_REGISTER_REQUEST                1
#define MSG_TYPE_REGISTER_SUCCESS                2
#define MSG_TYPE_REGISTER_FAILURE                3
/* Client/binder message types */
#define MSG_TYPE_LOC_REQUEST                     4
#define MSG_TYPE_LOC_SUCCESS                     5
#define MSG_TYPE_LOC_FAILURE                     6
/* Client/server message types */
#define MSG_TYPE_EXECUTE_REQUEST                 7
#define MSG_TYPE_EXECUTE_SUCCESS                 8
#define MSG_TYPE_EXECUTE_FAILURE                 9
/* Terminate message type */
#define MSG_TYPE_TERMINATE                       10

/**************** Message field maximum lengths ****************/
#define MAX_LENGTH_SERVER_IDENTIFIER             MAXHOSTNAMELEN
#define MAX_LENGTH_PORT                          sizeof(unsigned int)
#define MAX_LENGTH_NAME                          64
#define MAX_LENGTH_ARG_TYPE                      sizeof(int)
#define MAX_LENGTH_ARG_CHAR                      sizeof(char)
#define MAX_LENGTH_ARG_SHORT                     sizeof(short)
#define MAX_LENGTH_ARG_INT                       sizeof(int)
#define MAX_LENGTH_ARG_LONG                      sizeof(long)
#define MAX_LENGTH_ARG_DOUBLE                    sizeof(double)
#define MAX_LENGTH_ARG_FLOAT                     sizeof(float)
#define MAX_LENGTH_REASON_CODE                   sizeof(int)

/************** Argument type decoding constants ***************/
#define ARG_TYPE_IO_NATURE_MASK                  0xFF000000
#define ARG_TYPE_INFORMATION_MASK                0x00FF0000
#define ARG_TYPE_ARRAY_LENGTH_MASK               0x0000FFFF
#define ARG_TYPE_INFORMATION_SHIFT_AMOUNT        16

/************************ Success code *************************/
#define SUCCESS_CODE                             0

/************************ Warning codes ************************/
/* Server warning codes */
#define WARNING_CODE_DUPLICATED_PROCEDURE        1

/************************* Error codes *************************/
/* Network error codes */
#define ERROR_CODE_SOCKET_CREATION_FAILED        -1
#define ERROR_CODE_ADDR_INFO_NOT_FOUND           -2
#define ERROR_CODE_SOCKET_BINDING_FAILED         -3
#define ERROR_CODE_SOCKET_LISTENING_FAILED       -4
#define ERROR_CODE_SOCKET_ACCEPTING_FAILED       -5
#define ERROR_CODE_SOCKET_CONNECTING_FAILED      -6
#define ERROR_CODE_SOCKET_SENDING_FAILED         -7
#define ERROR_CODE_SOCKET_RECEIVING_FAILED       -8
#define ERROR_CODE_SOCKET_DESTRUCTION_FAILED     -9
#define ERROR_CODE_HOST_ADDRESS_NOT_FOUND        -10
#define ERROR_CODE_SOCKET_PORT_NOT_FOUND         -11
#define ERROR_CODE_BINDER_ADDRESS_NOT_FOUND      -12
#define ERROR_CODE_BINDER_PORT_NOT_FOUND         -13
/* Server error codes */
#define ERROR_CODE_WELCOME_SOCKET_NOT_CREATED    -14
#define ERROR_CODE_NOT_CONNECTED_TO_BINDER       -15
#define ERROR_CODE_NO_REGISTERED_PROCEDURES      -16
/* Binder error codes */
#define ERROR_CODE_PROCEDURE_NOT_FOUND           -17
#define ERROR_CODE_PROCEDURE_REGISTRATION_FAILED -18

// Object definitions needed by binder
struct server_info {
  std::string server_identifier;
  int port;
  int socket;

  server_info(std::string server_identifier, int port, int socket);
};

struct procedure_signature {
  std::string name;
  int *argTypes;

  procedure_signature(std::string name, int *argTypes);
  procedure_signature(const struct procedure_signature &r);
};

struct server_function_info {
  struct server_info *si;
  struct procedure_signature *ps;

  server_function_info(struct server_info *si, struct procedure_signature *ps);
};

struct ps_compare {
  bool operator() (const procedure_signature& lhs,
    const procedure_signature& rhs) const;
};

#endif
