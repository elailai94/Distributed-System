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

static map<procedure_signature, skeleton> proc_skele_dict;
string serverIdentifier;
unsigned int port;
//int binder_sock; //only one binder socket variable
int sock;

	//TODO:
  // CREATE SERVER
  // CONNECT TO BINDER

struct thread_data {
  int sock;
  vector<string> *buf;
};


void connect_to_binder() {
  char *serverAddress = getenv("BINDER_ADDRESS");
  char *port = getenv("BINDER_PORT");
  binder_sock = createConnection(serverAddress, port);
  return binder_sock;
}


int rpcRegister(char * name, int *argTypes, skeleton f){

  int binder_sock = connect_to_binder();
  int retStatus;

  RegisterRequestMessage request_message = new RegisterRequestMessage(serverIdentifier, port, name, argTypes);
  int status = success_message->send(binder_sock);

  if(status == 0){
    //Success
    Segment * segment = 0;
    status = Segment::receive(binder_sock, segment);

    if(segment->getType() == MSG_TYPE_REGISTER_SUCCESS){  
      Message * cast = segment->getMessage();
      RegisterSuccessMessage * rsm = dynamic_cast<RegisterSuccessMessage*>(cast);
    
      struct procedure_signature k(string(name), argTypes);
      proc_skele_dict[k] = f;
    
    }else if(segment->getType() == MSG_TYPE_REGISTER_FAILURE){
      return 0;
    }
  
  }else if( status > 0){
    //Warning
    return -99;
  }else if( status < 0){
    //Error
    return 99;
  }

}


int rpcExecute(void){
  //Create connection socket ot be used for accepting clients
  vector<int> myConnections;
  vector<int> myToRemove;
 
  fd_set readfds;
  int n;
  int status;
  struct sockaddr_storage their_addr;

  while(true){
    //CONNECTIONS VECTOR    
    FD_ZERO(&readfds); 
    FD_SET(sock, &readfds); 

    n = sock;

    for (vector<int>::iterator it = myConnections.begin();it != myConnections.end(); ++it) {
      int connection = *it;
      FD_SET(connection, &readfds);
       if (connection > n){
        n = connection;
      }
    } 
    
    n = n+1;

    status = select(n, &readfds, NULL, NULL, NULL);
    
    if (status == -1) {
      cerr << "ERROR: select failed." << endl;
    } else {
      
      if (FD_ISSET(sock, &readfds)) {       
        socklen_t addr_size = sizeof their_addr;
        int new_sock = accept(sock, (struct sockaddr*)&their_addr, &addr_size);

        if (new_sock < 0) {
          cerr << "ERROR: while accepting connection" << endl;
          close(new_sock);
          continue;
        }

        myConnections.push_back(new_sock);

      } else {

        for (vector<int>::iterator it = myConnections.begin(); it != myConnections.end(); ++it) {   
          int tempConnection = *it;         
          if (FD_ISSET(tempConnection, &readfds)) {

            int reasonCode = 0;
            Segment * segment = 0;
            status = Segment::receive(sock, segment);

            if(segment->getType() == MSG_TYPE_EXECUTE_REQUEST){
              Message * cast = segment->getMessage();
              ExecuteRequestMessage * eqm = dynamic_cast<ExecuteRequestMessage*>(cast);
    
              procedure_signature ps = new procedure_signature(eqm->getName(), eqm->getArgTypes());       
              skeleton skel = proc_skele_dict[ps];
              
              int result = skel(eqm->getArgTypes(), eqm->getArgs());

              if(result == 0 ){
                ExecuteSuccessMessage execute_success = new ExecuteSuccessMessage(name, eqm->getArgTypes(), eqm->getArgs());
                int status = execute_success->send(sock);
              }else{
                ExecuteFailureMessage execute_failure = new ExecuteFailureMessage(reasoncode);
                int status = execute_failure->send(sock);
              }
            }

            if (status == 0) {
              // client has closed the connection
              myToRemove.push_back(sock);
              return errorMsg;
            }

          }
        }
      }

      for (vector<int>::iterator it = myToRemove.begin(); it != myToRemove.end(); ++it) {
        myConnections.erase(remove(myConnections.begin(), myConnections.end(), *it), myConnections.end());
        close(*it);
      }
      myToRemove.clear();
    }
  }

  freeaddrinfo(servinfo);
}

int rpcInit(void){
  int status;
  struct addrinfo hints;
  struct addrinfo* servinfo;
  struct addrinfo* p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, "0", &hints, &servinfo);

  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 0;
  }

  p = servinfo;
  sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
 
  status = bind(sock, servinfo->ai_addr, servinfo->ai_addrlen);
  status = listen(sock, 5);
 
  char hostname[256];
  gethostname(hostname, 256);
  
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);

  getsockname(sock, (struct sockaddr *)&sin, &len);

  stringstream ss;
  ss << ntohs(sin.sin_port);
  string ps = ss.str();

  serverIdentifier = hostname;
  port =  ntohs(sin.sin_port);

}