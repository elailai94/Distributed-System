#include "server_functions.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>

#include <netdb.h>
#include <stdlib.h>

#include "segment.h"
#include "message.h"
#include "loc_success_message.h"
#include "loc_failure_message.h"
#include "loc_request_message.h"
#include "execute_success_message.h"
#include "execute_failure_message.h"
#include "execute_request_message.h"

#include "rpc.h"
#include "constants.h"
#include "helper_function.h"
#include "message_types.h"
#include "message_results.h"

using namespace std;

bool connectedToBinder = false;
int binder_sock;

int connectedToBinder(){

	if(connectedToBinder){
		return 0;		
	}

	char * binderAddressString = getenv ("BINDER_ADDRESS");
    char * binderPortString = getenv("BINDER_PORT");

  	if(binderAddressString == NULL){
        return 1;
    }

    if(binderPortString == NULL){
        return 2;
    }
    
    binder_sock = create_connection(binderAddressString, binderPortString);

    if (binder_sock < 0) {
        return 3;
    }else{
    	connectedToBinder = true;
    }

}


int sendExecute(int sock, char* name, int* argTypes, void**args){
    
    ExecuteRequestMessage execute_request = new ExecuteRequestMessage(name, argTypes, args);
    int status = execute_request->send(sock);	
    int returnVal;

	char* retName; 
	int* retArgTypes;
	void** retArgs

    if(status == 0){
		Segment * segment = 0;
        status = Segment::receive(sock, segment);
	
        if(segment->getType() == MSG_TYPE_EXECUTE_SUCCESS) {
	
			Message * cast = segment->getMessage();
			ExecuteSuccessMessage * esm = dynamic_cast<ExecuteSuccessMessage*>(cast);
    
			retName = esm->getName(); 
			retArgTypes = esm->getArgTypes();
			retArgs = esm->getArgs();

			if(strcmp(retName, name)){
				//extractArgumentsMessage(replyMessageP, argTypes, args, argTypesLength, false);
				returnVal = 0;
			}else{
				returnVal = 99;
			}

        }else if(segment->getType() ==  MSG_TYPE_EXECUTE_FAILURE){
			Message * cast = segment->getMessage();
			ExecuteFailureMessage * efm = dynamic_cast<ExecuteFailureMessage*>(cast);
        	returnVal = efm->getReasonCode();	
        }

    }else{ //Something bad happened
    	returnVal = 99;
    }

    return returnVal
}


int rpcCall(char * name, int * argTypes, void ** args) {

	int returnVal;
	char *serverAddress;
	char *serverPort;
	int server_socket

	if(!connectedToBinder){
		returnVal = connectedToBinder();
	}
	//do something with returnVal

	LocRequestMessage loc_request = new LocRequestMessage(name, argTypes);
  	int binder_status = loc_request->send(binder_sock); 
  	//maybe error check with binder_status

	/**Server stuff **/
	if(status == 0){
		Segment * segment = 0;
        status = Segment::receive(sock, segment);

		if(segment->getType() == MSG_TYPE_LOC_SUCCESS){ //'LOC_REQUEST'
    		Message * cast = segment->getMessage();
    		LocSuccessMessage * lcm = dynamic_cast<LocSuccessMessage*>(cast);
		  	
		  	serverAddress = lcm->getServerIdentifier();
		  	serverPort = lcm->getPort();

	  	}else if(segment->getType() == MSG_TYPE_LOC_FAILURE){
			//something bad happens
	  		return 1;
	  	}
	}

    int server_sock = create_connection(serverAddress, serverPort);
	int server_status = sendExecute(server_sock, name, argTypes, args);
	
	return server_status; 
}

