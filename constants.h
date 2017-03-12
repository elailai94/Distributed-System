#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <string>
#include <sys/param.h>

/************************ Message types ************************/
/* Server/binder message types */
#define MSG_TYPE_REGISTER_REQUEST         1
#define MSG_TYPE_REGISTER_SUCCESS         2
#define MSG_TYPE_REGISTER_FAILURE         3
/* Client/binder message types */
#define MSG_TYPE_LOC_REQUEST              4
#define MSG_TYPE_LOC_SUCCESS              5
#define MSG_TYPE_LOC_FAILURE              6
/* Client/server message types */
#define MSG_TYPE_EXECUTE_REQUEST          7
#define MSG_TYPE_EXECUTE_SUCCESS          8
#define MSG_TYPE_EXECUTE_FAILURE          9
/* Terminate message type */
#define MSG_TYPE_TERMINATE                10

/**************** Message field maximum lengths ****************/
#define MAX_LENGTH_SERVER_IDENTIFIER      MAXHOSTNAMELEN
#define MAX_LENGTH_PORT                   sizeof(unsigned int)
#define MAX_LENGTH_NAME                   64
#define MAX_LENGTH_ARG_TYPE               sizeof(int)
#define MAX_LENGTH_ARG_CHAR               sizeof(char)
#define MAX_LENGTH_ARG_SHORT              sizeof(short)
#define MAX_LENGTH_ARG_INT                sizeof(int)
#define MAX_LENGTH_ARG_LONG               sizeof(long)
#define MAX_LENGTH_ARG_DOUBLE             sizeof(double)
#define MAX_LENGTH_ARG_FLOAT              sizeof(float)
#define MAX_LENGTH_REASON_CODE            sizeof(int)

/************** Argument type decoding constants ***************/
#define ARG_TYPE_IO_NATURE_MASK           0xFF000000
#define ARG_TYPE_INFORMATION_MASK         0x00FF0000
#define ARG_TYPE_ARRAY_LENGTH_MASK        0x0000FFFF
#define ARG_TYPE_INFORMATION_SHIFT_AMOUNT 16

/************************* Error codes *************************/
/* Network error codes */
#define ERROR_CODE_SOCKET                 -2
#define ERROR_CODE_GETADDRINFO            -3
#define ERROR_CODE_BIND                   -4
#define ERROR_CODE_LISTEN                 -5
#define ERROR_CODE_ACCEPT                 -6
#define ERROR_CODE_CONNECT                -7
#define ERROR_CODE_SEND                   -8
#define ERROR_CODE_RECV                   -9
#define ERROR_CODE_CLOSE                  -10
#define ERROR_CODE_GETHOSTNAME            -11
#define ERROR_CODE_GETSOCKNAME            -12

// Constants used for binder.cc
struct server_info {
  std::string server_identifier;
  int port;
  int socket;

  server_info(std::string server_identifier, int port, int socket);
};

bool operator == (const server_info &l, const server_info &r);

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
