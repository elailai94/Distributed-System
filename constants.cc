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

bool ps_compare::operator() (const procedure_signature& lhs, const procedure_signature& rhs) const{

  if(lhs.name == rhs.name){
    int i = 0;
    int lArgs = lhs.argTypes[i];
    int rArgs = rhs.argTypes[i];

    while(lArgs!= 0 && rArgs != 0) {
      int lArraySize = lArgs & 0x1111;
      int rArraySize = rArgs & 0x1111;

      if (((lArgs & 0xFFFF0000) != (rArgs & 0xFFFF0000)) 
        || (lArraySize == 0 && rArraySize != 0) 
        || (lArraySize != 0 && rArraySize == 0)){
        
        return lArgs < rArgs;
      }

      i++;
   
      lArgs = lhs.argTypes[i];
      rArgs = rhs.argTypes[i];
    }

    if(rArgs == 0){
        return false;    
    } else {
        return true;
    }

  } else {
      return (lhs.name < rhs.name);
  }
}

