#include <string>

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

	server_info(string server_identifier, int port, int socket)
	  : server_identifier(server_identifier), port(port), socket(socket){}
};

struct procedure_signature {
	string name;
    int *argTypes;

	procedure_signature(string name, int *argTypes)
	  : name(name), argTypes(argTypes){}

    procedure_signature(const struct procedure_signature & r) {
        name = r.name;
        argTypes = r.argTypes;
    }
};

struct server_function_info {
	struct server_info *si;
	struct procedure_signature *ps;

	server_function_info(struct server_info *si, struct procedure_signature *ps)
	  : si(si), ps(ps){}
};

bool operator == (const server_info &l, const struct server_function_info  &r);
bool operator == (const server_function_info &l, const server_function_info &r);
bool operator == (const server_info &l, const server_info &r);
bool operator < (const procedure_signature &l, const procedure_signature &r);
