#include "server_functions.h"
#include <stdio.h>
#include <string.h>
#include <rpc.h>

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

#include <register_success_message.h>
#include <register_failure_message.h>
#include <register_request_message.h>
#include <execute_success_message.h>
#include <execute_failure_message.h>
#include <execute_request_message.h>

using namespace std;

static map<procedure_signature, skeleton> proc_skele_dict;
string serverIdentifier;
unsigned int port;
int binder_sock;
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
  int status = success_message.send(binder_sock);

  if(status == 0){
    //Success
    Message message; 
    Message::receive(binder_sock, message, retStatus);

    if(message.type == 2){  
      struct procedure_signature k(string(name), argTypes);
      proc_skele_dict[k] = f;
    }else if(message.type ==3){

    }

  
  }else if( status > 0){
    //Warning

  }else if( status < 0){
    //Error
    return status;
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

            Message message; 
            Message::receive(sock, message, status);

            if (status < 0) {
                RegisterFailureMessage failure_message = new RegisterFailureMessage(status);
                failure_message.send(sock);
                return errorMsg;
            }

            if (status == 0) {
              // client has closed the connection
              myToRemove.push_back(sock);
              return errorMsg;
            }

            request_handler(message, sock);
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