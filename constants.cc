#include "constants.h"
#include <cstring>
#include <vector>
#include <iostream>

using namespace std;

server_info::server_info(string server_identifier, int port, int socket)
  : server_identifier(server_identifier), port(port), socket(socket) {}

procedure_signature::procedure_signature(string name, int *argTypes)
  : name(name), argTypes(argTypes) {}

procedure_signature::procedure_signature(const struct procedure_signature & r) {
  name = r.name;
  argTypes = r.argTypes;
}

server_function_info::server_function_info(struct server_info *si,
  struct procedure_signature *ps)
  : si(si), ps(ps) {}
