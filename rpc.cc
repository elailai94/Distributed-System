#include <cstdio>
#include <cstring>
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
#include <cstdlib>

#include "segment.h"
#include "message.h"
#include "loc_success_message.h"
#include "loc_failure_message.h"
#include "loc_request_message.h"
#include "execute_success_message.h"
#include "execute_failure_message.h"
#include "execute_request_message.h"
#include "register_success_message.h"
#include "register_failure_message.h"
#include "register_request_message.h"
#include "terminate_message.h"

#include "rpc.h"
#include "constants.h"
#include "network.h"
#include "helper_function.h"

using namespace std;

//Global Variables for client
bool connectedToBinder = false;
int binderSocket = 0;

//Global Variables for server
struct addrinfo* servinfo;
static map<procedure_signature, skeleton, ps_compare> proc_skele_dict;

string serverIdentifier;
unsigned int port;
int welcomeSocket = 0;

void mapPrint(){

  cout << "Map Print: ";

  for(map<procedure_signature, skeleton, ps_compare>::const_iterator it = proc_skele_dict.begin(); it != proc_skele_dict.end(); ++it){
  
    cout << it->first.name << ", " ;
  }

  cout << endl;
}


// See interface (header file).
int rpcInit(){
	cout << "Running rpcInit..." << endl;

	// Creates a connection socket to be used for accepting connections
	// from clients
	welcomeSocket = createSocket();
	setUpToListen(welcomeSocket);

	// Opens a connection to the binder

	binderSocket = createSocket();
	string binderAddress = getBinderAddress();
	unsigned int binderPort = getBinderPort();
	setUpToConnect(binderSocket, binderAddress, binderPort);
  connectedToBinder = true;
  cout << "binderSocket: " << binderSocket << endl;

  return 0;
}

int sendExecute(int sock, string name, int* argTypes, void**args){

  ExecuteRequestMessage exeReqMsg = ExecuteRequestMessage(name, argTypes, args);
  Segment exeReqSeg = Segment(exeReqMsg.getLength(), MSG_TYPE_EXECUTE_REQUEST, &exeReqMsg);
  int status =  exeReqSeg.send(sock);

  int returnVal;

	string retName;
	int* retArgTypes;
	void** retArgs;

  if(status == 0){
    Segment * segment = 0;
    status = Segment::receive(sock, segment);

    if(segment->getType() == MSG_TYPE_EXECUTE_SUCCESS) {

			Message * cast = segment->getMessage();
	 		ExecuteSuccessMessage * esm = dynamic_cast<ExecuteSuccessMessage*>(cast);

			retName = esm->getName();
			retArgTypes = esm->getArgTypes();
			retArgs = esm->getArgs();

			if(retName == name){
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

  return returnVal;
}


int rpcCall(char * name, int * argTypes, void ** args) {

	string serverAddress;
	unsigned int serverPort;
	int status;

	if(!connectedToBinder){
		binderSocket = createSocket();
		string binderAddress = getBinderAddress();
		unsigned int binderPort = getBinderPort();
		status = setUpToConnect(binderSocket, binderAddress, binderPort);
	  connectedToBinder = true;
	}
	//do something with returnVal

  LocRequestMessage locReqMsg = LocRequestMessage(name, argTypes);
  Segment locReqSeg = Segment(locReqMsg.getLength(), MSG_TYPE_LOC_REQUEST, &locReqMsg);
  int binder_status = locReqSeg.send(binderSocket);

	//maybe error check with binder_status

	/**Server stuff **/
	if(binder_status == 0){
		Segment * segment = 0;
    status = Segment::receive(binderSocket, segment);

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

  int server_sock = createConnection(serverAddress, serverPort);
	int server_status = sendExecute(server_sock, string(name), argTypes, args);

	return server_status;
}



  //TODO:
  // CREATE SERVER
  // CONNECT TO BINDER

int rpcRegister(char * name, int *argTypes, skeleton f){

  RegisterRequestMessage regReqMsg = RegisterRequestMessage(serverIdentifier, port, name, argTypes);
  cout << "rpcRegister: " << name << endl;

  /*
  We should get seg.send to give us some feed back maybe
  int status = regReqMsg->send(binderSocket);
  */

  Segment regReqSeg = Segment(regReqMsg.getLength(), MSG_TYPE_REGISTER_REQUEST, &regReqMsg);
  int status = regReqSeg.send(binderSocket);
  cout << "rpcRegister Status: " << status << endl;

  if(status >= 0){
    //Success
    Segment *parsedSegment = 0;
    int result = 0;
    result = Segment::receive(binderSocket, parsedSegment);


    if(parsedSegment->getType() == MSG_TYPE_REGISTER_SUCCESS){

      cout << "MSG_TYPE_REGISTER_SUCCESS" << endl;

      Message * cast = parsedSegment->getMessage();
      //Error Checking maybe
      RegisterSuccessMessage * rsm = dynamic_cast<RegisterSuccessMessage*>(cast);

      struct procedure_signature k(string(name), argTypes);
      proc_skele_dict[k] = f;

      mapPrint();
    }else if(parsedSegment->getType() == MSG_TYPE_REGISTER_FAILURE){
      return 0;
    }

  }else if( status < 0){
    //Error
    return -99;
  }

  return 1;
}


int rpcExecute(void){
  //Create connection socket ot be used for accepting clients
  vector<int> myConnections;
  vector<int> myToRemove;

  fd_set readfds;
  int n;
  int status;
  struct sockaddr_storage their_addr;

  cout << "rpcExecute" << endl;


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
        socklen_t addr_size = sizeof their_addr;
        int new_sock = accept(welcomeSocket, (struct sockaddr*)&their_addr, &addr_size);

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
            status = Segment::receive(tempConnection, segment);

            if(segment->getType() == MSG_TYPE_EXECUTE_REQUEST){
              Message * cast = segment->getMessage();
              ExecuteRequestMessage * eqm = dynamic_cast<ExecuteRequestMessage*>(cast);

              procedure_signature * ps = new procedure_signature(eqm->getName(), eqm->getArgTypes());
              skeleton skel = proc_skele_dict[*ps];

              int result = skel(eqm->getArgTypes(), eqm->getArgs());

              if(result == 0 ){
                ExecuteSuccessMessage exeSuccessMsg = ExecuteSuccessMessage(eqm->getName(), eqm->getArgTypes(), eqm->getArgs());
                Segment exeSuccessSeg = Segment(exeSuccessMsg.getLength(), MSG_TYPE_EXECUTE_SUCCESS, &exeSuccessMsg);
                status = exeSuccessSeg.send(tempConnection);

              }else{
                ExecuteFailureMessage exeFailMsg = ExecuteFailureMessage(reasonCode);
                Segment exeFailSeg = Segment(exeFailMsg.getLength(), MSG_TYPE_EXECUTE_FAILURE, &exeFailMsg);
                status = exeFailSeg.send(tempConnection);
              }
            }

            if (status == 0) {
              // client has closed the connection
              myToRemove.push_back(tempConnection);
              return status;
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
  return 0;
}

int rpcCacheCall() {
	return 0;
}

int rpcTerminate() {
	cout << "Running rpcTerminate..." << endl;
	TerminateMessage messageToBinder = TerminateMessage();
	Segment segmentToBinder =
	  Segment(messageToBinder.getLength(), MSG_TYPE_TERMINATE, &messageToBinder);
	segmentToBinder.send(binderSocket);
	return 0;
}
