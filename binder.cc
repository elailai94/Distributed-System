#include <string.h>

#include <iostream>
#include <sstream>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <list>
#include <map>

#include <netdb.h>
#include <stdlib.h>

#include "segment.h"
#include "message.h"
#include "register_success_message.h"
#include "register_failure_message.h"
#include "register_request_message.h"
#include "loc_success_message.h"
#include "loc_failure_message.h"
#include "loc_request_message.h"

#include "constants.h"
#include "helper_function.h"
#include "message_types.h"
#include "message_results.h"

using namespace std;

//TODO:
//MANAGE CLIENT CONNECTION
//MANAGE SERVER CONNECTION
//DATABASE
//ROUND ROBIN

static map<procedure_signature, list<server_info *>, ps_compare> proc_loc_dict;
static list<server_function_info *> roundRobinList;

/*
TODO:
ADD FUNCTION TO MAP
ADD FUNCTION TO ROUND ROBIN
IF FUNCTION EXISTS IN ROUND ROBIN DELETE OLD REPLACE WITH NEW (where)
*/


void registration_request_handler(RegisterRequestMessage * message, int sock){
	const char * name = message->getName().c_str();
  int * argTypes = message->getArgTypes();
  string server_identifier = message->getServerIdentifier();
  int port = message->getPort();

  procedure_signature key(name, argTypes);

  int status = 0;

  //if 'key' dosnt exist in map, add it to the map and round robin
	if (proc_loc_dict.find(key) == proc_loc_dict.end()) {
    //The purpose of this function is so we can have copy of the argTypes that not the original
    int *memArgTypes = copyArgTypes(argTypes);

    key = procedure_signature(name, memArgTypes);
    proc_loc_dict[key] = list<server_info *>();

    //This is bad we shouldn't need a newKey and we should be able to use the key above
    //due to &* reasones I made a variable newKey for the 'info' object
    procedure_signature * newKey = new procedure_signature(name, memArgTypes);
    server_info * entry = new server_info(server_identifier, port, sock);

    //Adding to roundRobinList
    server_function_info * info = new server_function_info(entry, newKey);
    roundRobinList.push_back(info);

  } else {
    bool sameLoc = false;
    list<server_info *> hostList = proc_loc_dict[key];

    for (list<server_info *>::iterator it = hostList.begin(); it != hostList.end(); it++) {
      if((*it)->socket == sock){
        //If they have the same socket, then must be same server_address/port
        //The same procedure signature already exists on the same location
        //TODO: Move to end of round robin or something, maybe we should keep
        sameLoc = true;
      }
    }

  	if(!sameLoc){ //same procedure different socket
       server_info * new_msg_loc = new server_info(server_identifier, port, sock);
       hostList.push_back(new_msg_loc);
    }
  }

  RegisterSuccessMessage * success_message = new RegisterSuccessMessage(status);
  success_message->send(sock);
}

/*
TODO:
USE ROUND ROBIN TO ACCESS THE CORRECT SERVER/FUNCTION FOR THE CLIENT
*/
int location_request_handler(LocRequestMessage * message, int sock){

  bool exist = false;
	for (list<server_function_info *>::iterator it = roundRobinList.begin(); it != roundRobinList.end(); it++){

    //If the name are the same and argTypes
    if((*it)->ps->name == message->getName() && compareArr((*it)->ps->argTypes, message->getArgTypes() )){
      exist = true;

      LocSuccessMessage * success_message = new LocSuccessMessage((*it)->si->server_identifier, (*it)->si->port);
      success_message->send(sock);

      //When we have identified the correct procedure_signature use splice and move that service to the end
      roundRobinList.splice(roundRobinList.end(), roundRobinList, it);
      break;
 		}
	}

  if(!exist){
    int reasoncode = 0;
    LocFailureMessage * failure_message = new LocFailureMessage(reasoncode);
    failure_message->send(sock);
  }

	return 0; //LOC_FAILURE
}

int request_handler(Segment * segment, int sock){
  int retval = 0;
  if(segment->getType() == MSG_TYPE_REGISTER_REQUEST){ //'LOC_REQUEST'
    Message * cast1 = segment->getMessage();
    RegisterRequestMessage * rrm = dynamic_cast<RegisterRequestMessage*>(cast1);
    registration_request_handler(rrm, sock);

  }else if (segment->getType() == MSG_TYPE_LOC_REQUEST){ //'REGISTER'
    Message * cast2 = segment->getMessage();
    LocRequestMessage * lqm = dynamic_cast<LocRequestMessage*>(cast2);
    retval = location_request_handler(lqm, sock);
  }

	return retval;
}

// Returns the binder address
string getBinderAddress() {
   char hostname[MAXHOSTNAMELEN + 1] = {'\0'};
   gethostname(hostname, sizeof(hostname));
   return string(hostname);
}

// Returns the binder port
int getBinderPort(int welcomeSocket) {
   struct sockaddr_in binderAddress;
   socklen_t binderAddressLength = sizeof(binderAddress);

   int result = getsockname(welcomeSocket, (struct sockaddr*) &binderAddress,
      &binderAddressLength);
   if (result < 0) {
      exit(-1);
   } // if

   return ntohs(binderAddress.sin_port);
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

  p = servinfo;
  int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

  status = bind(sock, servinfo->ai_addr, servinfo->ai_addrlen);
  status = listen(sock, SOMAXCONN);

  cout << "BINDER_ADDRESS " << getBinderAddress() << endl;
  cout << "BINDER_PORT " << getBinderPort(sock) << endl;

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

            Segment * segment = 0;
            status = Segment::receive(sock, segment);

            //TODO: More sophisticaled error handling/replies
            if (status < 0) {
                RegisterFailureMessage * failure_message = new RegisterFailureMessage(status);
                failure_message->send(sock);
                return status;
            }

            if (status == 0) {
              // client has closed the connection
              myToRemove.push_back(sock);
              return status;
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
