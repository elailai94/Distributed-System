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
#include "helper_functions.h"

using namespace std;

//TODO:
//MANAGE CLIENT CONNECTION
//MANAGE SERVER CONNECTION
//DATABASE
//ROUND ROBIN

static map<procedure_signature, list<server_info *>, ps_compare> procLocDict;
static list<server_function_info *> roundRobinList;
static list<server_info *> serverList;

bool onSwitch = true;

/*
TODO:
ADD FUNCTION TO MAP
ADD FUNCTION TO ROUND ROBIN
IF FUNCTION EXISTS IN ROUND ROBIN DELETE OLD REPLACE WITH NEW (where)
*/

void mapPrint(){
  cout << "procLocDict size: "<<procLocDict.size() << endl;
  cout << "Map Print: ";
  for(map<procedure_signature, list<server_info *>, ps_compare> ::const_iterator it = procLocDict.begin();
   it != procLocDict.end(); it++){

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


void serverListPrint(){
  cout << "serverList Print: " << endl;
  for(list<server_info *> ::const_iterator it = serverList.begin(); it != serverList.end(); it++){
    cout << (*it)->server_identifier << ", " <<  (*it)->port << ", " <<  (*it)->socket<< endl;
  }
}

void registration_request_handler(RegisterRequestMessage * message, int sock){
  const char * name = message->getName().c_str();
  int * argTypes = message->getArgTypes();
  string server_identifier = message->getServerIdentifier();
  int port = message->getPort();

  cout << "We are trying to register: " << name  << ", " << server_identifier << ", " << port << endl;

  procedure_signature key(name, argTypes);

  int status = 0;

  //if 'key' dosnt exist in map, add it to the map and round robin
	if (procLocDict.find(key) == procLocDict.end()) {
    //The purpose of this function is so we can have copy of the argTypes that not the original
    int *memArgTypes = copyArgTypes(argTypes);

    key = procedure_signature(name, memArgTypes);
    procLocDict[key] = list<server_info *>();

    //This is bad we shouldn't need a newKey and we should be able to use the key above
    //due to &* reasones I made a variable newKey for the 'info' object
    procedure_signature * newKey = new procedure_signature(name, memArgTypes);
    server_info * entry = new server_info(server_identifier, port, sock);
    server_function_info * info = new server_function_info(entry, newKey);

    //Adding to roundRobinList if server is not found
    roundRobinList.push_back(info);

    //Adding to serverList if server is not found
    bool serverExist = false;
    for (list<server_info *>::iterator it = serverList.begin(); it != serverList.end(); it++) {
    
      if( (*it)->server_identifier == entry->server_identifier && (*it)->port == entry->port &&  (*it)->socket == entry->socket){
          serverExist = true;
          break;      
        }
    }

    cout << "entry->server_identifier: " <<entry->server_identifier << endl;
    cout << "entry->port: " <<entry->port << endl;
    cout << " entry->socket: " << entry->socket << endl;

    if(!serverExist){
      serverList.push_back(entry);
    }


  } else {
    bool sameLoc = false;
    list<server_info *> hostList = procLocDict[key];

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
  serverListPrint();
  
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
  string serverIdToPushBack;
  int portToPushBack;
  int socketToPushBack;

  //cout << "Hunted name names: " << message->getName() << endl;


	for (list<server_function_info *>::iterator it = roundRobinList.begin(); it != roundRobinList.end(); it++){
    //If the name are the same and argTypes
    //cout << "Iterator names: " << (*it)->ps->name << endl;

    if((*it)->ps->name == message->getName() && compareArr((*it)->ps->argTypes, message->getArgTypes() )){
      exist = true;

      //cout << "Attempt to send locSuccessMsg" << endl;
      //cout << "server_identifier: "<< (*it)->si->server_identifier << endl;
      //cout << "port: " << (*it)->si->port<< endl;

      serverIdToPushBack = (*it)->si->server_identifier;
      portToPushBack = (*it)->si->port;
      socketToPushBack = (*it)->si->socket;

      LocSuccessMessage locSuccessMsg = LocSuccessMessage((*it)->si->server_identifier.c_str(), (*it)->si->port);
      Segment locSuccessSeg = Segment(locSuccessMsg.getLength(), MSG_TYPE_LOC_SUCCESS, &locSuccessMsg);
      locSuccessSeg.send(sock);

      //When we have identified the correct procedure_signature use splice and move that service to the end
      roundRobinList.splice(roundRobinList.end(), roundRobinList, it);
      break;
 		}
	}

  if(exist){
    for (list<server_function_info *>::iterator it = roundRobinList.begin(); it != roundRobinList.end(); it++){

       if((*it)->si->server_identifier == serverIdToPushBack && (*it)->si->port == portToPushBack && (*it)->si->socket == socketToPushBack){
          roundRobinList.splice(roundRobinList.end(), roundRobinList, it);
       }
    }
  }else {
    int reasoncode = -5; // Need actual reasoncode
    LocFailureMessage locFailMsg = LocFailureMessage(reasoncode);
    Segment locFailSeg = Segment(locFailMsg.getLength(), MSG_TYPE_LOC_FAILURE, &locFailMsg);
    locFailSeg.send(sock);
  }
}


void binder_terminate_handler() {
  cout << "Binder set to execute" << endl;

  for (list<server_info *>::const_iterator it = serverList.begin(); it != serverList.end(); it++) {
    cout << "Terminating server: " << (*it)->server_identifier << ", " <<  (*it)->port << ", " <<  (*it)->socket<< endl;
    TerminateMessage termMsg = TerminateMessage();
    Segment termSeg = Segment(termMsg.getLength(), MSG_TYPE_TERMINATE, &termMsg);
    //termSeg.send((*it)->socket);

  }


  onSwitch = false;

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

  }else if (segment->getType() == MSG_TYPE_TERMINATE){

    cout << "Terminate Request" <<endl;

    binder_terminate_handler();
  }

	return retval;
}


//TODO:
//Create helper functions that can be used for rpcServer.cc
int main() {
// Code refactoring
/*
  fd_set allSockets;
  fd_set readSockets;

  // Clears all entries from the all sockets set and the read
  // sockets set
  FD_ZERO(&allSockets);
  FD_ZERO(&readSockets);

  // Creates the welcome socket, adds it to the all sockets set and
  // sets it as the maximum socket so far
  int welcomeSocket = createSocket();
  int status = setUpToListen(welcomeSocket);
  FD_SET(welcomeSocket, &allSockets);
  int maxSocket = welcomeSocket;

  // Prints the binder address and the binder port on the binder's
  // standard output
  cout << "BINDER_ADDRESS " << getHostAddress() << endl;
  cout << "BINDER_PORT " << getSocketPort(welcomeSocket) << endl;

  while (onSwitch) {
    readSockets = allSockets;

    // Checks if some of the sockets are ready to be read from
    int result = select(maxSocket + 1, &readSockets, 0, 0, 0);
    if (result < 0) {
      continue;
    }

    for (int i = 0; i <= maxSocket; i++) {
      if (!FD_ISSET(i, &readSockets)) {
        continue;
      }

      if (i == welcomeSocket) {

        // Creates the connection socket when a connection is made
        // to the welcome socket
        int connectionSocket = acceptConnection(i);
        if (connectionSocket < 0) {
          continue;
        }

        // Adds the connection socket to the all sockets set
        FD_SET(connectionSocket, &allSockets);

        // Sets the connection socket as the maximum socket so far
        // if necessary
        if (connectionSocket > maxSocket) {
          maxSocket = connectionSocket;
        }

      } else {

        // Creates a segment to receive data from the client/server and
        // reads into it from the connection socket
        Segment *segment = 0;
        result = 0;
        result = Segment::receive(i, segment);
        if (result <= 0) {
          // Closes the connection socket and removes it from the
          // all sockets set
          destroySocket(i);
          FD_CLR(i, &allSockets);
          continue;
        }

        request_handler(segment, i);
      }
    }
  }

  // Destroys the welcome socket
  destroySocket(welcomeSocket);
*/
  vector<int> myConnections;
  vector<int> myToRemove;

  // Creates the welcome socket
  int welcomeSocket = createSocket();
  int status = setUpToListen(welcomeSocket);

  cout << "BINDER_ADDRESS " << getHostAddress() << endl;
  cout << "BINDER_PORT " << getSocketPort(welcomeSocket) << endl;

  fd_set readfds;
  int n;

  while(onSwitch){
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
          if (FD_ISSET(tempConnection, &readfds)) {
            Segment * segment = 0;
            status = Segment::receive(tempConnection, segment);

            if (segment != 0){
              cout << "To be handled: "<< tempConnection << endl;
              if (segment->getType() == 0){
                cout << "getType() is null" << endl;
              }else{
                cout << segment->getType() << endl;
              }
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
