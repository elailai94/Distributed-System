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


bool operator < (const procedure_signature &l, const procedure_signature &r)
{
    if(l.name == r.name){
        int i = 0;
        int lArgs = l.argTypes[i];
        int rArgs = r.argTypes[i];

        // structs with less args are smaller
        // structs with the same args are the same
        // structs with the same number args,
        // but different arg values,
        // the one with the smaller value is smaller
        while(lArgs!= 0 && rArgs != 0){
            int lArraySize = lArgs & 0x1111;
            int rArraySize = rArgs & 0x1111;

            if (((lArgs & 0xFFFF0000) != (rArgs & 0xFFFF0000)) 
            	|| (lArraySize == 0 && rArraySize != 0) 
            	|| (lArraySize != 0 && rArraySize == 0)){
                return lArgs < rArgs;
            }
            i++;
            lArgs = l.argTypes[i];
            rArgs = r.argTypes[i];
        }

        if(rArgs == 0){ // || lArgs == 0
            // l is longer, so l > r
            // or same length with same parameters, so l == r, which means l < r is false
            return false;
        } else {
        	// r is longer, so l < r
            return true;
        }
    } else {
        // c > 0 if l > r
        // c < 0 if l < r
        return (l.name < r.name);
    }
}