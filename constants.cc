#include "constants.h"
#include <cstring>
#include <vector>
#include <iostream>

using namespace std;



server_info::server_info(string server_identifier, int port, int socket): server_identifier(server_identifier), port(port), socket(socket){}

procedure_signature::procedure_signature(string name, int *argTypes): name(name), argTypes(argTypes){}

procedure_signature::procedure_signature(const struct procedure_signature & r) {
    name = r.name;
    argTypes = r.argTypes;
}

server_function_info::server_function_info(struct server_info *si, struct procedure_signature *ps) : si(si), ps(ps){}

bool operator == (const server_function_info &l, const server_function_info &r) {
    //return (l.si->server_identifier == r.si->server_identifier && l.si->port == r.si->port && (!(*(l.ps) < *(r.ps)) && !(*(r.ps) < *(l.ps))));
	return (l.si->server_identifier == r.si->server_identifier && l.si->port == r.si->port);
}

bool operator == (const server_info &l, const server_function_info &r) {
    return (l.server_identifier == r.si->server_identifier && l.port == r.si->port);
}

bool operator == (const server_info &l, const server_info &r) {
    return (l.server_identifier == r.server_identifier && l.port == r.port);
}

