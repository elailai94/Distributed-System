
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

struct ps_compare {

  	bool operator() (const procedure_signature& lhs, const procedure_signature& rhs) const;

};
