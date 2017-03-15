#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string>
#include <list>
#include <map>
#include <iostream>

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

// Global variables for binder
static map<procedure_signature, list<server_info *>, ps_compare> procLocDict;
static list<server_function_info *> roundRobinList;
static list<server_info *> serverList;
static bool isTerminated = false;

// Handles a registration request from the server
void handleRegistrationRequest(RegisterRequestMessage *message, int sock) {
  const char *name = message->getName().c_str();
  int *argTypes = message->getArgTypes();
  string server_identifier = message->getServerIdentifier();
  int port = message->getPort();

  procedure_signature key(name, argTypes);

  int status = SUCCESS_CODE;

  // if 'key' doesn't exist in map, add it to the map and round robin
	if (procLocDict.find(key) == procLocDict.end()) {

    /*
     * The purpose of this function is so we can have copy of the argTypes
     * that is not the original
     */
    int *memArgTypes = copyArgTypes(argTypes);

    key = procedure_signature(name, memArgTypes);
    procLocDict[key] = list<server_info *>();

    // This is bad we shouldn't need a newKey and we should be able to use the key above
    // due to &* reasones I made a variable newKey for the 'info' object
    procedure_signature *newKey = new procedure_signature(name, memArgTypes);
    server_info *entry = new server_info(server_identifier, port, sock);
    server_function_info *info = new server_function_info(entry, newKey);

    // Adding to roundRobinList if server is not found
    roundRobinList.push_back(info);

    // Adding to serverList if server is not found
    bool serverExist = false;
    for (list<server_info *>::iterator it = serverList.begin(); it != serverList.end(); it++) {

      if ((*it)->server_identifier == entry->server_identifier &&
         (*it)->port == entry->port &&
         (*it)->socket == entry->socket) {
        serverExist = true;
        break;
      }
    }

    if (!serverExist) {
      serverList.push_back(entry);
    }

  } else {
    bool sameLoc = false;
    list<server_info *> hostList = procLocDict[key];

    for (list<server_info *>::iterator it = hostList.begin(); it != hostList.end(); it++) {

      if((*it)->server_identifier == server_identifier &&
         (*it)->port == port &&
         (*it)->socket == sock) {

        // If they have the same socket, then must be same server_address/port
        // The same procedure signature already exists on the same location
        sameLoc = true;
        status = WARNING_CODE_DUPLICATED_PROCEDURE;
      }
    }

  	if (!sameLoc) { // same procedure different socket

      server_info *new_msg_loc = new server_info(server_identifier, port, sock);
      hostList.push_back(new_msg_loc);

      int *newArgTypes = copyArgTypes(argTypes);

      procedure_signature *useFulKey = new procedure_signature(name, newArgTypes);
      server_function_info *info = new server_function_info(new_msg_loc, useFulKey);

      // Adding to roundRobinList if server is not found
      roundRobinList.push_back(info);

      // Adding to serverList if server is not found
      bool serverExist = false;
      for (list<server_info *>::iterator it = serverList.begin(); it != serverList.end(); it++) {

        if ((*it)->server_identifier == new_msg_loc->server_identifier &&
           (*it)->port == new_msg_loc->port &&
           (*it)->socket == new_msg_loc->socket) {
          serverExist = true;
          break;
        }
      }

      if (!serverExist) {
        serverList.push_back(new_msg_loc);
      }
    }
  }

  RegisterSuccessMessage messageToServer = RegisterSuccessMessage(status);
  Segment segmentToServer =
    Segment(messageToServer.getLength(), MSG_TYPE_REGISTER_SUCCESS,
      &messageToServer);
  segmentToServer.send(sock);
}

// Handles a location request from the client
void handleLocationRequest(LocRequestMessage *message, int sock) {
  bool exist = false;
  string serverIdToPushBack;
  int portToPushBack;
  int socketToPushBack;

	for (list<server_function_info *>::iterator it = roundRobinList.begin(); it != roundRobinList.end(); it++){
    //If the name are the same and argTypes

    if((*it)->ps->name == message->getName() && compareArr((*it)->ps->argTypes, message->getArgTypes() )){
      exist = true;

      serverIdToPushBack = (*it)->si->server_identifier;
      portToPushBack = (*it)->si->port;
      socketToPushBack = (*it)->si->socket;

      LocSuccessMessage locSuccessMsg = LocSuccessMessage((*it)->si->server_identifier.c_str(), (*it)->si->port);
      Segment locSuccessSeg = Segment(locSuccessMsg.getLength(), MSG_TYPE_LOC_SUCCESS, &locSuccessMsg);
      locSuccessSeg.send(sock);

      break;
 		}
	}

  if (exist) {

    list<server_function_info *>::iterator i = roundRobinList.begin();
    list<server_function_info *> tempList;

    while (i != roundRobinList.end()){
      if((*i)->si->server_identifier == serverIdToPushBack && (*i)->si->port == portToPushBack && (*i)->si->socket == socketToPushBack){
        tempList.push_back(*i);
        roundRobinList.erase(i++);  // alternatively, i = items.erase(i);
      }else{
        ++i;
      }
    }

    roundRobinList.splice(roundRobinList.end(), tempList);

  } else {
    LocFailureMessage locFailMsg =
      LocFailureMessage(ERROR_CODE_PROCEDURE_NOT_FOUND);
    Segment locFailSeg = Segment(locFailMsg.getLength(), MSG_TYPE_LOC_FAILURE, &locFailMsg);
    locFailSeg.send(sock);
  }
}

// Handles a termination request from the client
void handleTerminationRequest() {
  // Informs all the servers to terminate
  for (list<server_info *>::const_iterator it = serverList.begin(); it != serverList.end(); it++) {
    TerminateMessage messageToServer = TerminateMessage();
    Segment segmentToServer =
      Segment(messageToServer.getLength(), MSG_TYPE_TERMINATE,
        &messageToServer);
    segmentToServer.send((*it)->socket);
  }

  // Signals the binder to terminate
  isTerminated = true;
}

// Handles a request from the client/server
void handleRequest(Segment *segment, int socket) {
  switch (segment->getType()) {
    case MSG_TYPE_REGISTER_REQUEST: {
      RegisterRequestMessage *messageFromServer =
        dynamic_cast<RegisterRequestMessage *>(segment->getMessage());
      handleRegistrationRequest(messageFromServer, socket);
      break;
    }

    case MSG_TYPE_LOC_REQUEST: {
      LocRequestMessage *messageFromClient =
        dynamic_cast<LocRequestMessage *>(segment->getMessage());
      handleLocationRequest(messageFromClient, socket);
      break;
    }

    case MSG_TYPE_TERMINATE: {
      handleTerminationRequest();
      break;
    }
  }
}

int main() {
  fd_set allSockets;
  fd_set readSockets;

  /*
   * Clears all entries from the all sockets set and the read
   * sockets set
   */
  FD_ZERO(&allSockets);
  FD_ZERO(&readSockets);

  /*
   * Creates the welcome socket, adds it to the all sockets set and
   * sets it as the maximum socket so far
   */
  int welcomeSocket = createSocket();
  if (welcomeSocket < 0) {
    return welcomeSocket;
  }
  int result = setUpToListen(welcomeSocket);
  if (result < 0) {
    return result;
  }
  FD_SET(welcomeSocket, &allSockets);
  int maxSocket = welcomeSocket;

  /*
   * Prints the binder address and the binder port on the binder's
   * standard output
   */
  string binderIdentifier = getHostAddress();
  if (binderIdentifier == "") {
    return ERROR_CODE_HOST_ADDRESS_NOT_FOUND;
  }
  int port = getSocketPort(welcomeSocket);
  if (port < 0) {
    return port;
  }
  cout << "BINDER_ADDRESS " << binderIdentifier << endl;
  cout << "BINDER_PORT " << port << endl;

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
         * Creates a segment to receive data from the client/server and
         * reads into it from the connection socket
         */
        Segment *segment = 0;
        result = 0;
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

        // Handles a request from the client/server
        handleRequest(segment, i);
      }
    }
  }

  // Destroys the welcome socket
  destroySocket(welcomeSocket);

  return SUCCESS_CODE;
}
