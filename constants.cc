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

server_function_info::server_function_info(struct server_info *si, struct procedure_signature *ps) : si(si), ps(ps){}

bool ps_compare::operator() (const procedure_signature& l, const procedure_signature& r) const{

if(l.name == r.name)
    {
        int i = 0;
        int lArgs = l.argTypes[i];
        int rArgs = r.argTypes[i];

        // structs with less args are smaller
        // structs with the same args are the same
        // structs with the same number args,
        // but different arg values,
        // the one with the smaller value is smaller
        while(lArgs!= 0 && rArgs != 0)
        {
            int lArraySize = lArgs & 0x1111;
            int rArraySize = rArgs & 0x1111;

            if (((lArgs & 0xFFFF0000) != (rArgs & 0xFFFF0000)) || (lArraySize == 0 && rArraySize != 0) || (lArraySize != 0 && rArraySize == 0))
            {
                return lArgs < rArgs;
            }
            i++;
            lArgs = l.argTypes[i];
            rArgs = r.argTypes[i];
        }

        if(rArgs == 0) // || lArgs == 0
        {
            // l is longer, so l > r
            // or same length with same parameters, so l == r, which means l < r is false
            return false;
        }
        else
        {
            // r is longer, so l < r
            return true;
        }
    }
    else
    {
        // c > 0 if l > r
        // c < 0 if l < r
        return (l.name < r.name);
    }
}

