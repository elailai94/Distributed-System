#include "server_functions.h"
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
#include <list>
#include <map>

#include <netdb.h>
#include <stdlib.h>

#include "message.h"
#include "register_success_message.h"
#include "register_failure_message.h"
#include "register_request_message.h"
#include "loc_success_message.h"
#include "loc_failure_message.h"
#include "loc_request_message.h"

#include "rpc.h"
#include "constants.h"
#include "helper_function.h"

using namespace std;

//TODO:
//MANAGE CLIENT CONNECTION
//MANAGE SERVER CONNECTION
//DATABASE
//ROUND ROBIN


static map<procedure_signature, list<server_info *> * > proc_loc_dict;
static list<server_function_info *> roundRobinList;

/* 
TODO:
ADD FUNCTION TO MAP
ADD FUNCTION TO ROUND ROBIN
IF FUNCTION EXISTS IN ROUND ROBIN DELETE OLD REPLACE WITH NEW (where)
*/
void registration_request_handler(RegisterRequestMessage message, int sock){
	char * name = message.getName();
  int * argTypes = message.getArgTypes();  
  string server_identifier = message.getServerIdentifier();
  int port = message.getPort();
  
  procedure_signature * key = new procedure_signature(name, argTypes);
	
  server_info * new_msg_loc = new server_info(server_identifier, port, sock);

  int status = 0;

  //if no key dosnt exist in map
	if (proc_loc_dict.find(key) == proc_loc_dict.end()) {

    //Adding to the map
    int *memArgTypes = copyArgTypes(argTypes);
    key = procedure_signature(name, memArgTypes);
    proc_loc_dict[key] = new list<server_info *>();
    server_info * entry = new server_info(server_identifier, port, sock);

    //Adding to roundRobinList

    server_function_info * info = new server_function_info(entry, key);    
    roundRobinList.push_back(info);

  }else{	 	
    
    bool sameLoc = false;
    list<service_info *> *hostList = proc_loc_dict.find[key]; 

    for (list<service_info *>::iterator it = hostList->begin(); it != hostList->end(); it++) {
      if(it == new_msg_loc){
        //The same procedure signature already exists on the same location
        //TODO: Move to end of round robin or something
        sameLoc = true;
      }
    }
    //Same procedure signature, different location
  	if(!sameLoc){       
       hostList.push_back(new_msg_loc);
    }
  }

  RegisterSuccessMessage * success_message = new RegisterSuccessMessage(status);
  success_message.send(sock);
}

/*
TODO:
USE ROUND ROBIN TO ACCESS THE CORRECT SERVER/FUNCTION FOR THE CLIENT
*/
int location_request_handler(LocRequestMessage message, int sock){
 
  bool exist = false; 
	for (list<server_function_info *>::iterator it = roundRobinList->begin(); it != roundRobinList->end(); it++){
		//If the name are the same
    if(it->ps->name == message->name && compareArr(it->ps->arrTypes, message->arrTypes)){ 
		  //When we have identified the correct procedure_signature use round robin and move that service to the end
		  roundRobinList.splice(roundRobinList.end(), roundRobinList, it);
      exist = true;
      break;
 		}
	}

  if(exist){
    LocSuccessMessage * success_message = new LocSuccessMessage(it->server_identifier, it->port);
    success_message->send(sock);
  }else{
    LocFailureMessage * failure_message = new LocFailureMessage(0);
    failure_message->send(sock);    
  }

	return 0; //LOC_FAILURE
}(

int request_handler(Segment segment, int sock){
  int retval;
  if(segment.getType() == MSG_TYPE_LOC_REQUEST){ //'LOC_REQUEST' 
    retval = registration_request_handler(segment.getMessage(), sock);
  }else if (segment.getType() == MSG_TYPE_REGISTER_REQUEST){ //'REGISTER'
    retval = location_request_handler(segment.getMessage(), sock);
  }

	return retval;
}


//TODO:
//Create helper functions that can be used for rpcServer.cc
int main(){

  vector<int> myConnections;
  vector<int> myToRemove;

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
  int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
 
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

  cout << "BINDER_ADDRESS " << hostname << endl;
  cout << "BINDER_PORT " << ntohs(sin.sin_port) << endl;

  fd_set readfds;
  int n;
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

            Segment segment; 
            Segment::receive(sock, segment, status);

            if (status < 0) {
                RegisterFailureMessage * failure_message = new RegisterFailureMessage(status);
                failure_message.send(sock);
                return errorMsg;
            }

            if (status == 0) {
              // client has closed the connection
              myToRemove.push_back(sock);
              return errorMsg;
            }

            request_handler(segment, sock);
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
