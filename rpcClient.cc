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

int  send_LOC_REQUEST(string name, int argTypes[]){
    int status = connectToBinder();

    if (status == 0) {
        Sender s(binderSocketFd);
        status = s.sendLocRequestMessage(name, argTypes);
    }

    return status;
}



int connect_Binder() {
    char * binderAddressString = getenv ("BINDER_ADDRESS");
    char * binderPortString = getenv("BINDER_PORT");

    if(binderAddressString == NULL){

        return INIT_UNSET_BINDER_ADDRESS;
    }

    if(binderPortString == NULL){
        return INIT_UNSET_BINDER_PORT;
    }
    
    if (binderSocketFd < 0) {
        return INIT_BINDER_SOCKET_FAILURE;
    }
    return 0;
}



int rpcCall(char * name, int * argTypes, void ** args) {
	char * binderAddressString = getenv ("BINDER_ADDRESS");
    char * binderPortString = getenv("BINDER_PORT");
    int binder_sock = create_connection(binderAddressString, binderPortString);

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
	
	return 
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
			//TODO Maybe we can double check these retVals are same as what we input
        	return 0; 
        }else if(message.type ==  MSG_TYPE_EXECUTE_FAILURE){
        	return message.getReasonCode();	
        }

    }else{ //Something bad happened
    	return 1;
    }
}

