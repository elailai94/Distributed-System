#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <list>
#include <map>

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

// Global variables for server
static map<procedure_signature, skeleton, ps_compare> procSkeleDict;
static string serverIdentifier;
static int port = -1;
static int welcomeSocket = -1;
static int serverBinderSocket = -1;
static bool onSwitch = true;

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

	/*
   * Creates a welcome socket to be used for accepting connections
	 * from clients
   */
	welcomeSocket = createSocket();
  if (welcomeSocket < 0) {
    return welcomeSocket;
  }
	int result = setUpToListen(welcomeSocket);
  if (result < 0) {
    return result;
  }
	serverIdentifier = getHostAddress();
  if (serverIdentifier == "") {
    return ERROR_CODE_HOST_ADDRESS_NOT_FOUND;
  }
	port = getSocketPort(welcomeSocket);
  if (port < 0) {
    return port;
  }

	// Opens a connection to the binder
	serverBinderSocket = createSocket();
  if (serverBinderSocket < 0) {
    return serverBinderSocket;
  }
	string binderAddress = getBinderAddress();
  if (binderAddress == "") {
    return ERROR_CODE_BINDER_ADDRESS_NOT_FOUND;
  }
	int binderPort = getBinderPort();
  if (binderPort < 0) {
    return binderPort;
  }
	result = setUpToConnect(serverBinderSocket, binderAddress, binderPort);
  if (result < 0) {
    return result;
  }

  cout << "This servers serverBinderSocket is: " << serverBinderSocket << endl;

  return SUCCESS_CODE;
}

// See interface (header file).
int rpcCall(char *name, int *argTypes, void **args) {
  cout << "Running rpcCall..." << endl;

	string serverAddress;
	unsigned int serverPort = 0;

  // Opens a connection to the binder
	int clientBinderSocket = createSocket();
  if (clientBinderSocket < 0) {
    return clientBinderSocket;
  }
	string binderAddress = getBinderAddress();
  if (binderAddress == "") {
    return ERROR_CODE_BINDER_ADDRESS_NOT_FOUND;
  }
	int binderPort = getBinderPort();
  if (binderPort < 0) {
    return binderPort;
  }
	int result = setUpToConnect(clientBinderSocket, binderAddress, binderPort);
  if (result < 0) {
    return result;
  }

  //cout << "Error was handled" << endl;
  /*
   * Sends a location request message to the binder to locate the server for
   * the procedure
   */
  LocRequestMessage messageToBinder = LocRequestMessage(string(name), argTypes);
  Segment segmentToBinder =
	  Segment(messageToBinder.getLength(), MSG_TYPE_LOC_REQUEST, &messageToBinder);
  result = segmentToBinder.send(clientBinderSocket);
  if (result < 0) {
    return result;
  }

  //cout << "Loc was sent" << endl;

  /*
   * Receives a location response message from the binder indicating whether
   * the retrieval of the server location is successful or not
   */
  Segment *segmentFromBinder = 0;
  result = Segment::receive(clientBinderSocket, segmentFromBinder);
  if (result < 0) {
    return result;
  }

 //cout << "result was recieved: " << result << endl;

  result = SUCCESS_CODE;
	switch (segmentFromBinder->getType()) {
		case MSG_TYPE_LOC_SUCCESS: {
      LocSuccessMessage *messageFromBinder =
				dynamic_cast<LocSuccessMessage *>(segmentFromBinder->getMessage());
		  serverAddress = messageFromBinder->getServerIdentifier();
			serverPort = messageFromBinder->getPort();
      break;
		}

		case MSG_TYPE_LOC_FAILURE: {
      LocFailureMessage *messageFromBinder =
        dynamic_cast<LocFailureMessage *>(segmentFromBinder->getMessage());
			result = messageFromBinder->getReasonCode();
			break;
		}
	}
  if (result < 0) {
    return result;
  }

  destroySocket(clientBinderSocket); // Closes the connection to the binder

  // Opens a connection to the server
  int serverSocket = createSocket();
  if (serverSocket < 0) {
    return serverSocket;
  }
	result = setUpToConnect(serverSocket, serverAddress, serverPort);
  if (result < 0) {
    return result;
  }

  cout << "Server Socket: " << serverSocket << endl;
  cout << "Server Address: " << serverAddress << endl;
  cout << "Server Port: " << serverPort << endl;
  cout << "Connected to server..." << endl;

  /*
   * Sends an execute request message to the server to execute the
   * procedure
   */
  ExecuteRequestMessage messageToServer =
    ExecuteRequestMessage(string(name), argTypes, args);
  Segment segmentToServer =
    Segment(messageToServer.getLength(), MSG_TYPE_EXECUTE_REQUEST,
      &messageToServer);
  result = messageToServer.send(serverSocket);
  if (result < 0) {
    return result;
  }

  /*
   * Receives a execution response message from the binder indicating
   * whether the execution of the procedure is successful or not
   */
  Segment *segmentFromServer = 0;
  result = Segment::receive(serverSocket, segmentFromServer);
  if (result < 0) {
    return result;
  }

  result = SUCCESS_CODE;
  switch (segmentFromServer->getType()) {
    case MSG_TYPE_EXECUTE_SUCCESS: {
      ExecuteSuccessMessage *messageFromServer =
        dynamic_cast<ExecuteSuccessMessage *>(segmentFromServer->getMessage());
      void** newArgs = messageFromServer->getArgs();

      unsigned int numOfArgs =
        countNumOfArgTypes(messageFromServer->getArgTypes()) - 1;
      for (unsigned int i = 0; i < numOfArgs; i++) {
        args[i] = newArgs[i];
      }

      break;
    }

    case MSG_TYPE_EXECUTE_FAILURE: {
      ExecuteFailureMessage *messageFromServer =
        dynamic_cast<ExecuteFailureMessage*>(segmentFromServer->getMessage());
      result = messageFromServer->getReasonCode();
      break;
    }
  }

  destroySocket(serverSocket); // Closes the connection to the server
  cout << "Got here..." << endl;
  return result;
}

// TODO:
// CREATE SERVER
// CONNECT TO BINDER
// See interface (header file).
int rpcRegister(char *name, int *argTypes, skeleton f){
  cout << "Running rpcRegister..." << endl;

  /*
   * Checks if this server is connected to the binder already
   * (i.e.: rpcInit is called before rpcRegister)
   */
  if (serverBinderSocket < 0) {
    return ERROR_CODE_NOT_CONNECTED_TO_BINDER;
  }

  /*
   * Sends a register request message to the binder informing it
   * that a server procedure with the indicated name and list of
   * argument types is available at this server
   */
  RegisterRequestMessage messageToBinder =
    RegisterRequestMessage(serverIdentifier, port, string(name), argTypes);
  Segment segmentToBinder =
    Segment(messageToBinder.getLength(), MSG_TYPE_REGISTER_REQUEST, &messageToBinder);
  int result = segmentToBinder.send(serverBinderSocket);
  if (result < 0) {
    return result;
  }

  /*
   * Receives a register response message from the binder indicating
   * whether the registration of a server procedure is successful
   * or not
   */
  Segment *segmentFromBinder = 0;
  result = Segment::receive(serverBinderSocket, segmentFromBinder);
  if (result < 0) {
    return result;
  }

  result = SUCCESS_CODE;
  switch (segmentFromBinder->getType()) {
    case MSG_TYPE_REGISTER_SUCCESS: {
      RegisterSuccessMessage *messageFromBinder =
        dynamic_cast<RegisterSuccessMessage *>(segmentFromBinder->getMessage());
      result = messageFromBinder->getReasonCode();

      /*
       * Makes an entry in a local database, associating the server skeleton
       * with the name and list of argument types
       */
      struct procedure_signature k = procedure_signature(string(name), argTypes);
      procSkeleDict[k] = f;
      cout << "k: " << k.name << ", "<< f << endl;
      break;
    }

    case MSG_TYPE_REGISTER_FAILURE: {
      RegisterFailureMessage *messageFromBinder =
        dynamic_cast<RegisterFailureMessage *>(segmentFromBinder->getMessage());
      result = messageFromBinder->getReasonCode();
      break;
    }
  }

  return result;
}

// See interface (header file).
int rpcExecute(){
  cout << "Running rpcExecute..." << endl;
  /*
   * Checks if this server has created a welcome socket already
   * (i.e.: rpcInit is called before rpcExecute)
   */
  if (welcomeSocket < 0) {
    return ERROR_CODE_WELCOME_SOCKET_NOT_CREATED;
  }

  /*
   * Checks if this server is connected to the binder already
   * (i.e.: rpcInit is called before rpcExecute)
   */
  if (serverBinderSocket < 0) {
    return ERROR_CODE_NOT_CONNECTED_TO_BINDER;
  }

  /*
   * Checks if there are any registered procedures to serve
   * (i.e.: rpcRegister is called before rpcExecute)
   */
  if (procSkeleDict.size() == 0) {
    return ERROR_CODE_NO_REGISTERED_PROCEDURES;
  }

  fd_set allSockets;
  fd_set readSockets;

  /*
   * Clears all entries from the all sockets set and the read
   * sockets set
   */
  FD_ZERO(&allSockets);
  FD_ZERO(&readSockets);

  /*
   * Adds the welcome socket to the all sockets set and sets
   * it as the maximum socket so far
   */
  FD_SET(serverBinderSocket, &allSockets);
  FD_SET(welcomeSocket, &allSockets);
  int maxSocket = welcomeSocket;

  cout << "Execute init was fine" << endl;

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
        cout << "New" << endl;
        /*
         * Creates the connection socket when a connection is made
         * to the welcome socket
         */
        int connectionSocket = acceptConnection(i);
        if (connectionSocket < 0) {
          continue;
        }

        // Adds the connection socket to the all sockets set
        FD_SET(connectionSocket, &allSockets);

        /*
         * Sets the connection socket as the maximum socket so far
         * if necessary
         */
        if (connectionSocket > maxSocket) {
          maxSocket = connectionSocket;
        }

      } else {

        cout << "Old" << endl;
        /*
         * Creates a segment to receive data from the client/binder and
         * reads into it from the connection socket
         */
        Segment *segment = 0;
        result = Segment::receive(i, segment);
        if (result < 0) {
          /*
           * Closes the connection socket and removes it from the
           * all sockets set
           */
          cout << "Bad result, I'm closing, i is: "<< i << endl;
          destroySocket(i);
          FD_CLR(i, &allSockets);
          continue;
        }

        switch (segment->getType()) {
          case MSG_TYPE_EXECUTE_REQUEST: {
            ExecuteRequestMessage *messageFromClient =
              dynamic_cast<ExecuteRequestMessage *>(segment->getMessage());
            procedure_signature *ps =
              new procedure_signature(messageFromClient->getName(),
                messageFromClient->getArgTypes());

            cout << "messageFromClient->getName(): " << messageFromClient->getName() << endl;
            printArgTypes(messageFromClient->getArgTypes());
            printArgs(messageFromClient->getArgTypes(), messageFromClient->getArgs());

            skeleton skel = procSkeleDict[*ps];

            if (skel == 0) {
              cout << "Skel is null" << endl;
            }

            result = skel(messageFromClient->getArgTypes(), messageFromClient->getArgs());

            cout << "Result: " << result << endl;
            printArgs(messageFromClient->getArgTypes(), messageFromClient->getArgs());

            if (result == 0) {

              ExecuteSuccessMessage messageToClient =
                ExecuteSuccessMessage(messageFromClient->getName(),
                  messageFromClient->getArgTypes(), messageFromClient->getArgs());
              Segment segmentToClient =
                Segment(messageToClient.getLength(), MSG_TYPE_EXECUTE_SUCCESS,
                  &messageToClient);
              int tstatus = segmentToClient.send(i);
              cout << "ExecuteSuccessMessage status: " << tstatus << endl;

            } else {

              ExecuteFailureMessage messageToClient = ExecuteFailureMessage(result);
              Segment segmentToClient =
                Segment(messageToClient.getLength(), MSG_TYPE_EXECUTE_FAILURE,
                  &messageToClient);
              segmentToClient.send(i);

            }

            break;
          }

          case MSG_TYPE_TERMINATE: {
            cout << "Got to terminate!" << endl;
            // Checks if the termination request comes from the binder (?)
            if (i != serverBinderSocket) {
               continue;
            }
            onSwitch = false;
            break;
          }
        }

      }
    }
  }

  // Destroys the welcome socket
  destroySocket(welcomeSocket);
  cout << "We are destroying the welcomeSocket: " << welcomeSocket << endl;

  return SUCCESS_CODE;
}

// See interface (header file).
int rpcCacheCall() {
	return SUCCESS_CODE;
}

// See interface (header file).
int rpcTerminate() {
	cout << "Running rpcTerminate..." << endl;

  // Opens a connection to the binder
  int clientBinderSocket = createSocket();
  if (clientBinderSocket < 0) {
    return clientBinderSocket;
  }
  string binderAddress = getBinderAddress();
  if (binderAddress == "") {
    return ERROR_CODE_BINDER_ADDRESS_NOT_FOUND;
  }
  int binderPort = getBinderPort();
  if (binderPort < 0) {
    return binderPort;
  }
  int result = setUpToConnect(clientBinderSocket, binderAddress, binderPort);
  if (result < 0) {
    return result;
  }

  // Sends a terminate message to the binder
  TerminateMessage messageToBinder = TerminateMessage();
  Segment segmentToBinder =
    Segment(messageToBinder.getLength(), MSG_TYPE_TERMINATE, &messageToBinder);
  result = segmentToBinder.send(clientBinderSocket);

  // Closes the connection to the binder
  destroySocket(clientBinderSocket);

	return SUCCESS_CODE;
}
