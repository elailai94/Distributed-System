#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <unistd.h>
#include <map>
#include <vector>

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
static bool isTerminated = false;

// See interface (header file).
int rpcInit() {
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

  return SUCCESS_CODE;
}

// See interface (header file).
int rpcCall(char *name, int *argTypes, void **args) {
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

  /*
   * Receives a location response message from the binder indicating whether
   * the retrieval of the server location is successful or not
   */
  Segment *segmentFromBinder = 0;
  result = Segment::receive(clientBinderSocket, segmentFromBinder);
  destroySocket(clientBinderSocket); // Closes the connection to the binder
  if (result < 0) {
    return result;
  }

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

  // Opens a connection to the server
  int serverSocket = createSocket();
  if (serverSocket < 0) {
    return serverSocket;
  }
	result = setUpToConnect(serverSocket, serverAddress, serverPort);
  if (result < 0) {
    return result;
  }

  /*
   * Sends an execute request message to the server to execute the
   * procedure
   */
  ExecuteRequestMessage messageToServer =
    ExecuteRequestMessage(string(name), argTypes, args);
  Segment segmentToServer =
    Segment(messageToServer.getLength(), MSG_TYPE_EXECUTE_REQUEST,
      &messageToServer);
  result = segmentToServer.send(serverSocket);
  if (result < 0) {
    return result;
  }

  /*
   * Receives a execution response message from the binder indicating
   * whether the execution of the procedure is successful or not
   */
  Segment *segmentFromServer = 0;
  result = Segment::receive(serverSocket, segmentFromServer);
  destroySocket(serverSocket); // Closes the connection to the server
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

  return result;
}

// See interface (header file).
int rpcRegister(char *name, int *argTypes, skeleton f) {
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

// Handles the execution request from the client
void *handleExecutionRequest(void *args) {
  /*
   * Unpacks the single parameter passed to this function into multiple
   * arguments
   */
  void **argsArray = (void **) args;
  string skelName = *((string *) argsArray[0]);
  int *skelArgTypes = (int *) argsArray[1];
  void **skelArgs = (void **) argsArray[2];
  long skelSocket = (long) argsArray[3];
  skeleton skelSkeleton = *((skeleton *) argsArray[4]);

  /*
   * Hands over control to the skeleton, which is expected to unmarshall
   * the message, call the appropriate procedures as requested by the
   * clients, and marshall the returns
   */
  int result = skelSkeleton(skelArgTypes, skelArgs);

  if (result == SUCCESS_CODE) {

    ExecuteSuccessMessage messageToClient =
      ExecuteSuccessMessage(skelName, skelArgTypes, skelArgs);
    Segment segmentToClient =
      Segment(messageToClient.getLength(), MSG_TYPE_EXECUTE_SUCCESS,
        &messageToClient);
    segmentToClient.send(skelSocket);

  } else {

    ExecuteFailureMessage messageToClient = ExecuteFailureMessage(result);
    Segment segmentToClient =
      Segment(messageToClient.getLength(), MSG_TYPE_EXECUTE_FAILURE,
        &messageToClient);
    segmentToClient.send(skelSocket);

  }

  // Terminates the current thread
  pthread_exit(0);
}

// See interface (header file).
int rpcExecute() {
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
  vector<pthread_t> allThreads;

  /*
   * Clears all entries from the all sockets set and the read
   * sockets set
   */
  FD_ZERO(&allSockets);
  FD_ZERO(&readSockets);

  /*
   * Adds the welcome socket and the binder socket to the all sockets set
	 * and sets the binder socket as the maximum socket so far
   */
	FD_SET(welcomeSocket, &allSockets);
  FD_SET(serverBinderSocket, &allSockets);
  int maxSocket = serverBinderSocket;

  while (!isTerminated) {
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
            skeleton skel = procSkeleDict[*ps];

            /*
             * Packs the arguments into a single parameter to be passed
             * to the new thread
             */
            void **handleExecutionRequestArgs = new void*[5]();
            handleExecutionRequestArgs[0] =
              (void *) new string(messageFromClient->getName());
            handleExecutionRequestArgs[1] =
              (void *) messageFromClient->getArgTypes();
            handleExecutionRequestArgs[2] =
              (void *) messageFromClient->getArgs();
            handleExecutionRequestArgs[3] = (void *) i;
            handleExecutionRequestArgs[4] = (void *) &skel;

            /*
             * Creates a new thread to handle the execution request
             * from the client
             */
            pthread_t newThread;
            pthread_create(&newThread, 0, handleExecutionRequest,
              (void *) handleExecutionRequestArgs);
            allThreads.push_back(newThread);

            break;
          }

          case MSG_TYPE_TERMINATE: {
            // Checks if the termination request comes from the binder
            if (i != serverBinderSocket) {
               continue;
            }

            // Waits for all threads to terminate
            for (unsigned int i = 0; i < allThreads.size(); i++) {
              pthread_join(allThreads[i], 0);
            }

            // Signals the server to terminate
            isTerminated = true;

            break;
          }
        }

      }
    }
  }

  // Closes the connection to the binder
	destroySocket(serverBinderSocket);

  // Destroys the welcome socket
  destroySocket(welcomeSocket);

  return SUCCESS_CODE;
}

// See interface (header file).
int rpcCacheCall() {
	return SUCCESS_CODE;
}

// See interface (header file).
int rpcTerminate() {
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
  if (result < 0) {
    return result;
  }

  // Closes the connection to the binder
  destroySocket(clientBinderSocket);

	return SUCCESS_CODE;
}
