#include <cstdio>
#include <cstring>
#include <iostream>
#include <bitset>
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
#include "helper_functions.h"

using namespace std;

// Global variables for client
bool connectedToBinder = false;
int binderSocket = 0;

// Global variables for server
static map<procedure_signature, skeleton, ps_compare> procSkeleDict;
string serverIdentifier;
unsigned int port = 0;
int welcomeSocket = 0;

void mapPrint(){
  cout << "procSkeleDict size: "<<procSkeleDict.size() << endl;

  cout << "Map Print: ";

  for(map<procedure_signature, skeleton>::iterator it = procSkeleDict.begin();
   it != procSkeleDict.end(); ++it){

    cout << it->first.name << ", ";
  }

  cout << endl;
}


void printArgTypes(int * argTypes){
  cout << " Printing argTypes: " ;

  unsigned num = countNumOfArgTypes(argTypes);

  for(int i = 0; i < num; i++){
    cout << argTypes[i] << ", ";
  }
  cout << endl;
}


void printArgs(int * argTypes, void  ** args){
  cout << "Printing args: " ;

  // Parses the argument from the buffer
  unsigned numOfArgs = countNumOfArgTypes(argTypes) - 1;


  cout<<"Number of args: " << numOfArgs << endl;

  for (unsigned int i = 0; i < numOfArgs; i++) {
    int argType = argTypes[i];
    int argTypeInformation =
      (argType & ARG_TYPE_INFORMATION_MASK) >> ARG_TYPE_INFORMATION_SHIFT_AMOUNT;
    int argTypeArrayLength = argType & ARG_TYPE_ARRAY_LENGTH_MASK;
    argTypeArrayLength = (argTypeArrayLength == 0) ? 1: argTypeArrayLength;
    void *arg = args[i];

    switch (argTypeInformation) {
      case ARG_CHAR: {
        cout << "Printing ARG_CHAR: " << endl;
        char *argCharArray = (char *) arg;
        cout << argCharArray << endl;
        break;
      }

      case ARG_SHORT: {
        cout << "Printing ARG_SHORT: " << endl;
        short *argShortArray = (short *) arg;
        for (int j = 0; j < argTypeArrayLength; j++) {
          cout << argShortArray[i] << ", ";
        }
        cout << endl;
        break;
      }

      case ARG_INT: {
        cout << "Printing ARG_INT: " << endl;
        int *argIntArray = (int *) arg;
        for (int j = 0; j < argTypeArrayLength; j++) {
          cout << argIntArray[j] << ", ";
        }
        cout << endl;
        break;
      }

      case ARG_LONG: {
        cout << "Printing ARG_LONG: " << endl;
        long *argLongArray = (long *) arg;
        for (int j = 0; j < argTypeArrayLength; j++) {
          cout << argLongArray[j] << ",";
        }
        cout << endl;
        break;
      }

      case ARG_DOUBLE: {
        cout << "Printing ARG_DOUBLE: " << endl;
        double *argDoubleArray = (double *) arg;
        for (int j = 0; j < argTypeArrayLength; j++) {
          cout << argDoubleArray[j] << ", ";
        }
        cout << endl;
        break;
      }

      case ARG_FLOAT: {
        cout << "Printing ARG_FLOAT: " << endl;
        float *argFloatArray = (float *) arg;
        for (int j = 0; j < argTypeArrayLength; j++) {
          cout << argFloatArray[j] << ", ";
        }
        cout << endl;
        break;
      }
    }
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
	serverIdentifier = getHostAddress();
	port = getSocketPort(welcomeSocket);

	// Opens a connection to the binder
	binderSocket = createSocket();
	string binderAddress = getBinderAddress();
	unsigned int binderPort = getBinderPort();
	setUpToConnect(binderSocket, binderAddress, binderPort);
  connectedToBinder = true;

  return 0;
}

// See interface (header file).
int rpcCall(char *name, int *argTypes, void **args) {
  cout << "Running rpcCall..." << endl;

	string serverAddress;
	unsigned int serverPort = 0;
	int status;

	if(!connectedToBinder){
    cout << "Connecting to binder..." << endl;
		binderSocket = createSocket();
		string binderAddress = getBinderAddress();
		unsigned int binderPort = getBinderPort();
		status = setUpToConnect(binderSocket, binderAddress, binderPort);
	  connectedToBinder = true;
    cout << "Connected to binder..." << endl;
	}
	//do something with returnVal

  LocRequestMessage messageToBinder = LocRequestMessage(string(name), argTypes);
  Segment segmentToBinder =
	  Segment(messageToBinder.getLength(), MSG_TYPE_LOC_REQUEST, &messageToBinder);
  int binder_status = segmentToBinder.send(binderSocket);
  cout << "LOC REQUEST message sent..." << endl;

	//maybe error check with binder_status
  //TODO: SEGMENT FAULT IF NOT IN THIS FOR LOOP
	/**Server stuff **/
	if(binder_status >= 0){

    Segment *parsedSegment = 0;
    int tempStatus = 0;
    tempStatus = Segment::receive(binderSocket, parsedSegment);

    Message *messageFromBinder = parsedSegment->getMessage();
		switch (parsedSegment->getType()) {
			case MSG_TYPE_LOC_SUCCESS: {
				cout << "LOC SUCCESS message received..." << endl;
        LocSuccessMessage *lsm =
				  dynamic_cast<LocSuccessMessage *>(messageFromBinder);
				serverAddress = lsm->getServerIdentifier();
				serverPort = lsm->getPort();
				cout << "serverAddress: " << serverAddress << endl;
        cout << "serverPort: " << serverPort << endl;
        break;
			}

			case MSG_TYPE_LOC_FAILURE: {
        cout << "LOC FAILURE message received..." << endl;
				return -1;
				break;
			}
		}
	}

  cout << "Connecting to server..." << endl;
  int serverSocket = createSocket();
	int status1 = setUpToConnect(serverSocket, serverAddress, serverPort);

  cout << "status1: " << status1 << endl;
  cout << "Server Socket: " << serverSocket << endl;
  cout << "Server Address: " << serverAddress << endl;
  cout << "Server Port: " << serverPort << endl;
  cout << "Connected to server..." << endl;

  ExecuteRequestMessage exeReqMsg = ExecuteRequestMessage(name, argTypes, args);
  Segment exeReqSeg = Segment(exeReqMsg.getLength(), MSG_TYPE_EXECUTE_REQUEST, &exeReqMsg);
  int status2 =  exeReqSeg.send(serverSocket);

  cout << "Status of exeRegMsg send: " << status2 << endl;

  int returnVal = 0;


  Segment * parsedSegmentEsm = 0;
  int status3 = 0;
  status3 = Segment::receive(serverSocket, parsedSegmentEsm);
  cout << "Flag 1" << endl;

  //instead we have

  cout << "Flag 2" << endl;
  cout << parsedSegmentEsm->getType() << endl;

  switch (parsedSegmentEsm->getType()) {
    case MSG_TYPE_EXECUTE_SUCCESS: {
      cout << "EXECUTE SUCCESS message received..." << endl;

      Message * msg = parsedSegmentEsm->getMessage();
      ExecuteSuccessMessage * esm = dynamic_cast<ExecuteSuccessMessage*>(msg);

      // TODO FIX: name = esm->getName();
      //argTypes = esm->getArgTypes();
      void** newArgs = esm->getArgs();
      unsigned numOfArgs = countNumOfArgTypes(esm->getArgTypes()) - 1;

      for (unsigned int i = 0; i < numOfArgs; i++) {
        args[i] = newArgs[i];
      }

      break;
    }

    case MSG_TYPE_EXECUTE_FAILURE: {
      cout << "EXECUTE FAILURE message received..." << endl;

      Message * cast = parsedSegmentEsm->getMessage();
      ExecuteFailureMessage * efm = dynamic_cast<ExecuteFailureMessage*>(cast);
      returnVal = efm->getReasonCode();
      break;
    }
  }

  return returnVal;
}


  //TODO:
  // CREATE SERVER
  // CONNECT TO BINDER

int rpcRegister(char * name, int *argTypes, skeleton f){
  cout << "Running rpcExecute..." << endl;
  RegisterRequestMessage regReqMsg = RegisterRequestMessage(serverIdentifier, port, name, argTypes);

  /*
  cout << "rpcRegister name: " << name << endl;
  cout << "rpcRegister serverIdentifier: " << serverIdentifier << endl;
  cout << "rpcRegister port: " << port << endl;
  */


  /*
  We should get seg.send to give us some feed back maybe
  int status = regReqMsg->send(binderSocket);
  */

  Segment regReqSeg = Segment(regReqMsg.getLength(), MSG_TYPE_REGISTER_REQUEST, &regReqMsg);
  int status = regReqSeg.send(binderSocket);

  //cout << "rpcRegister Status: " << status << endl;

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

      //struct procedure_signature k(string(name), argTypes);
      struct procedure_signature k = procedure_signature(string(name), argTypes);

      procSkeleDict[k] = f;

      cout << "k: " << k.name << ", "<< f << endl;

    }else if(parsedSegment->getType() == MSG_TYPE_REGISTER_FAILURE){
      return 0;
    }

  }else if( status < 0){
    //Error
    return -99;
  }

  return 1;
}


int rpcExecute(){
	cout << "Running rpcExecute..." << endl;
  mapPrint();

  //Create connection socket ot be used for accepting clients
  vector<int> myConnections;
  vector<int> myToRemove;

  fd_set readfds;
  int n;
  int status;

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


          if (FD_ISSET(tempConnection, &readfds)) {

            int reasonCode = 0;
            Segment * segment = 0;
            status = Segment::receive(tempConnection, segment);

            if (status < 0) {
              myToRemove.push_back(tempConnection);
              cerr << "ERROR: receive failed" << endl;
              return -169;
            }

            if (status == 0) {
              // client has closed the connection
              myToRemove.push_back(tempConnection);
              return status;
            }

            if(segment->getType() == MSG_TYPE_EXECUTE_REQUEST){
              Message * cast = segment->getMessage();
              ExecuteRequestMessage * erm = dynamic_cast<ExecuteRequestMessage*>(cast);

              procedure_signature * ps = new procedure_signature(erm->getName(), erm->getArgTypes());

              cout << "erm->getName(): " << erm->getName() << endl;
              printArgTypes(erm->getArgTypes());
              printArgs(erm->getArgTypes(), erm->getArgs());

              skeleton skel = procSkeleDict[*ps];

              if(skel == 0){
                cout << "Skel is null" << endl;
              }

              int result = skel(erm->getArgTypes(), erm->getArgs());

              cout << "Result: " << result << endl;
              printArgs(erm->getArgTypes(), erm->getArgs());

              if(result == 0 ){
                ExecuteSuccessMessage exeSuccessMsg = ExecuteSuccessMessage(erm->getName(), erm->getArgTypes(), erm->getArgs());
                Segment exeSuccessSeg = Segment(exeSuccessMsg.getLength(), MSG_TYPE_EXECUTE_SUCCESS, &exeSuccessMsg);
                status = exeSuccessSeg.send(tempConnection);
                cout << "ExecuteSuccessMessage status: " << status << endl;

              }else{
                ExecuteFailureMessage exeFailMsg = ExecuteFailureMessage(reasonCode);
                Segment exeFailSeg = Segment(exeFailMsg.getLength(), MSG_TYPE_EXECUTE_FAILURE, &exeFailMsg);
                status = exeFailSeg.send(tempConnection);
              }
            
            }else if(segment->getType() == MSG_TYPE_TERMINATE){
              break; 
              //and other clean up
            }
             

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

  destroySocket(welcomeSocket);
  return 0;
}

int rpcCacheCall() {
  cout << "Running rpcCacheCall..." << endl;
	return 0;
}

int rpcTerminate() {
	cout << "Running rpcTerminate..." << endl;

  // Sends a terminate message to the binder
	TerminateMessage messageToBinder = TerminateMessage();
	Segment segmentToBinder =
	  Segment(messageToBinder.getLength(), MSG_TYPE_TERMINATE, &messageToBinder);
	segmentToBinder.send(binderSocket);

  sleep(1);
  // Closes the connection to the binder
  destroySocket(binderSocket);

	return 0;
}
