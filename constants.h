#include <string>

using namespace std;

//Constants used for binder.cc

struct server_info {     
	string server_identifier;
	int port;
	int socket;
	
	server_info(string server_identifier, int port) : server_identifier(server_identifier), port(port){}
}; 

struct procedure_signature {     
	string name;
    int *argTypes;

	procedure_signature(string name, int *argTypes) : name(name), argTypes(argTypes){}
}; 

struct server_function_info {     
	server_info si;
	procedure_signature ps;

	server_function_info(server_info si, procedure_signature ps): si(si), port(port), socket(socket), ps(ps){}
};
