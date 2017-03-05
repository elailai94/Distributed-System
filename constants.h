#ifdef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <string>
#include <vector>

using namespace std;


struct thread_data {
  int sock;
  vector<string> *buf;
};

// Constants used for binder.cc
struct server_info {
	string server_identifier;
	int port;
	int socket;

	server_info(string server_identifier, int port, int socket);
};

struct procedure_signature {
	string name;
    int *argTypes;

	procedure_signature(string name, int *argTypes);

    procedure_signature(const struct procedure_signature & r);
};

struct server_function_info {
	struct server_info *si;
	struct procedure_signature *ps;

	server_function_info(struct server_info *si, struct procedure_signature *ps);
};

bool operator == (const server_info &l, const struct server_function_info  &r);
bool operator == (const server_function_info &l, const server_function_info &r);
bool operator == (const server_info &l, const server_info &r);
bool operator < (const procedure_signature &l, const procedure_signature &r);

#endif
