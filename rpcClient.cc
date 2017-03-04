#include "server_functions.h"
#include <stdio.h>
#include <string.h>
#include <rpc.h>

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

using namespace std;

bool connectedToBinder = false;

int connectedToBinder(){

	if(connectedToBinder){
		return 0;		
	}

	char * binderAddressString = getenv ("BINDER_ADDRESS");
    char * binderPortString = getenv("BINDER_PORT");
    int binder_sock;

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

int getServerSocket(){

}


int rpcCall(char * name, int * argTypes, void ** args) {

	int returnVal;

	if(!connectedToBinder){
		returnVal = connectedToBinder();
	}
	//do something with returnVal

	LocRequestMessage loc_request = new LocRequestMessage(name. argTypes);
  	int binder_status = execute_request.send(binder_sock); 

	//**Server stuff **/
	char *serverAddress;
	char *serverPort;
	int server_socket

	if(status == 0){
	  Message message; 
	  Message::receive(sock, message, status);

	  if(message.type == MSG_TYPE_LOC_SUCCESS) {
	  	serverAddress = message.getServerIdentifier();
	  	serverPort = message.getPort();
	  }else if(message.type == MSG_TYPE_LOC_FAILURE){
		//something bad happens
	  	return 1;
	  }
	}

    int server_sock = create_connection(serverAddress, serverPort);
	int server_status = sendExecute(server_sock, name, argTypes, args);
	
	return server_status; 
}


int sendExecute(int sock, char* name, int* argTypes, void**args){
    
    ExecuteRequestMessage execute_request = new ExecuteRequestMessage(name, argTypes, args);
    int status = execute_request.send(sock);	
    int returnVal;

	char* retName; 
	int* retArgTypes;
	void** retArgs

    if(status == 0){
	    Message message; 
        Message::receive(sock, message, status);

        if(message.type == MSG_TYPE_EXECUTE_SUCCESS) {
			retName = message.getName(); 
			retArgTypes = message.getArgTypes();
			retArgs = message.getArgs();

			if(strcmp(retName, name)){
				//extractArgumentsMessage(replyMessageP, argTypes, args, argTypesLength, false);
				returnVal = 0;
			}else{
				returnVal = 99;
			}

        }else if(message.type ==  MSG_TYPE_EXECUTE_FAILURE){
        	returnVal = message.getReasonCode();	
        }

    }else{ //Something bad happened
    	returnVal = 99;
    }

    return returnVal
}

