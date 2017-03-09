#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>
#include <vector>
#include <algorithm>
#include <list>
#include <map>

#include <cstdlib>
#include <cstring>

#include "segment.h"
#include "message.h"
#include "register_success_message.h"
#include "register_failure_message.h"
#include "register_request_message.h"
#include "loc_success_message.h"
#include "loc_failure_message.h"
#include "loc_request_message.h"
#include "terminate_message.h"

#include "constants.h"
#include "network.h"
#include "helper_function.h"

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

void mapPrint(){
  cout << "proc_loc_dict size: "<<proc_loc_dict.size() << endl;
  cout << "Map Print: ";
  for(map<procedure_signature, list<server_info *>, ps_compare> ::const_iterator it = proc_loc_dict.begin();
   it != proc_loc_dict.end(); it++){

    cout << it->first.name << ", " ;
  }

  cout << endl;
}

void roundRobinPrint(){
  cout << "roundRobin Print: ";
  for(list<server_function_info *> ::const_iterator it = roundRobinList.begin(); it != roundRobinList.end(); it++){
    cout << (*it)->ps->name << endl;
  }
}

void registration_request_handler(RegisterRequestMessage * message, int sock){
  const char * name = message->getName().c_str();
  int * argTypes = message->getArgTypes();
  string server_identifier = message->getServerIdentifier();
  int port = message->getPort();

  cout << "We are trying to register: " << name << endl;

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

  //mapPrint();
  //roundRobinPrint();

  RegisterSuccessMessage regSuccessMsg = RegisterSuccessMessage(status);
  Segment regSuccessSeg = Segment(regSuccessMsg.getLength(), MSG_TYPE_REGISTER_SUCCESS, &regSuccessMsg);
  regSuccessSeg.send(sock);

}

/*
TODO:
USE ROUND ROBIN TO ACCESS THE CORRECT SERVER/FUNCTION FOR THE CLIENT
*/
void location_request_handler(LocRequestMessage * message, int sock){

  bool exist = false;

  cout << "Hunted name names: " << message->getName() << endl;


	for (list<server_function_info *>::iterator it = roundRobinList.begin(); it != roundRobinList.end(); it++){
    //If the name are the same and argTypes
    if((*it)->ps->name == message->getName() && compareArr((*it)->ps->argTypes, message->getArgTypes() )){
      exist = true;

      LocSuccessMessage locSuccessMsg = LocSuccessMessage((*it)->si->server_identifier, (*it)->si->port);
      Segment locSuccessSeg = Segment(locSuccessSeg.getLength(), MSG_TYPE_LOC_SUCCESS, &locSuccessMsg);
      locSuccessSeg.send(sock);

      //When we have identified the correct procedure_signature use splice and move that service to the end
      roundRobinList.splice(roundRobinList.end(), roundRobinList, it);
      break;
 		}

    cout << "Iterator names: " << (*it)->ps->name << endl;
	}

  if(!exist){
    int reasoncode = -5; // Need actual reasoncode
    LocFailureMessage locFailMsg = LocFailureMessage(reasoncode);
    Segment locFailSeg = Segment(locFailMsg.getLength(), MSG_TYPE_LOC_FAILURE, &locFailMsg);
    locFailSeg.send(sock);
  }
}

int request_handler(Segment * segment, int sock){
  int retval = 0;
  if(segment->getType() == MSG_TYPE_REGISTER_REQUEST){ 
    Message * cast1 = segment->getMessage();
    RegisterRequestMessage * rrm = dynamic_cast<RegisterRequestMessage*>(cast1);

    registration_request_handler(rrm, sock);

  }else if (segment->getType() == MSG_TYPE_LOC_REQUEST){ 
    
    cout << "Loc Request" << endl;

    Message * cast2 = segment->getMessage();
    LocRequestMessage * lqm = dynamic_cast<LocRequestMessage*>(cast2);

    location_request_handler(lqm, sock);
  }

	return retval;
}

//TODO:
//Create helper functions that can be used for rpcServer.cc
int main() {
  vector<int> myConnections;
  vector<int> myToRemove;

  // Creates the welcome socket
  int welcomeSocket = createSocket();
  int status = setUpToListen(welcomeSocket);

  cout << "BINDER_ADDRESS " << getHostAddress() << endl;
  cout << "BINDER_PORT " << getSocketPort(welcomeSocket) << endl;

  fd_set readfds;
  int n;

  while(true){
    //CONNECTIONS VECTOR
    FD_ZERO(&readfds);
    FD_SET(welcomeSocket, &readfds);

    n = welcomeSocket;

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

      if (FD_ISSET(welcomeSocket, &readfds)) {
        int connectionSocket = acceptConnection(welcomeSocket);

        if (connectionSocket < 0) {
          cerr << "ERROR: while accepting connection" << endl;
          continue;
        }

        myConnections.push_back(connectionSocket);

      } else {

        for (vector<int>::iterator it = myConnections.begin(); it != myConnections.end(); ++it) {
          int tempConnection = *it;

            cout << "Socket iterator" << endl;

          if (FD_ISSET(tempConnection, &readfds)) {

            cout << "We in here" << endl;

            Segment * segment = 0;
            status = Segment::receive(tempConnection, segment);

            //TODO: More sophisticaled error handling/replies
            // Maybe status should be reasonacode instead
            if (status < 0 && segment->getType() == MSG_TYPE_REGISTER_REQUEST) {
                RegisterFailureMessage regFailMsg = RegisterFailureMessage(status);
                Segment regFailSeg = Segment(regFailMsg.getLength(), MSG_TYPE_REGISTER_FAILURE, &regFailMsg);
                regFailSeg.send(tempConnection);
                return status;
            } else if (status < 0 && segment->getType() == MSG_TYPE_LOC_REQUEST) {
                LocFailureMessage locFailMsg = LocFailureMessage(status);
                Segment regFailSeg = Segment(locFailMsg.getLength(), MSG_TYPE_LOC_FAILURE, &locFailMsg);
                regFailSeg.send(tempConnection);
                return status;
            }

            if (status == 0) {
              // client has closed the connection
              myToRemove.push_back(tempConnection);
              return status;
            }

            request_handler(segment, tempConnection);
          }
        }
      }

      for (vector<int>::iterator it = myToRemove.begin(); it != myToRemove.end(); ++it) {
        myConnections.erase(remove(myConnections.begin(), myConnections.end(), *it), myConnections.end());
        destroySocket(*it);
      }
      myToRemove.clear();
    }
  }

  // Destroys the welcome socket
  destroySocket(welcomeSocket);
}
