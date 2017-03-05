#include "constants.h"
#include <cstring>

#include <iostream>

using namespace std;

bool operator == (const server_function_info &l, const server_function_info &r) {
    return (l.si.server_identifier == r.si.server_identifier && l.si.port == r.si.port && (!(*(l.ps) < *(r.ps)) && !(*(r.ps) < *(l.ps))));
}

bool operator == (const server_info &l, const server_function_info &r) {
    return (l.server_identifier == r.server_identifier && l.port == r.port);
}

bool operator == (const server_info &l, const server_info &r) {
    return (l.server_identifier == r.server_identifier && l.port == r.port);
}

