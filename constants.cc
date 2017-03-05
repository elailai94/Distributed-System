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

bool ps_compare::operator() (const procedure_signature& lhs, const procedure_signature& rhs) const{

	if(lhs.name == rhs.name){
  		
		int * array1 = lhs.argTypes;
		int * array2 = rhs.argTypes;

  		while (*array1 != 0 || *array2 != 0){
	      if (*array1 != *array2){
	          cout << "Flag1" << endl;
	          return true;
	      }
	      
	      *array1++;
	      *array2++;
	  	}

		cout << "Flag1.5" << endl;		
		return false;
	}
	
	cout << "Flag2" << endl;
	return true;
}