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

void *SendToServer(void *threadarg) {
  struct thread_data *my_data;
  my_data = (struct thread_data *) threadarg;

  vector<string> *dataQueue = my_data->buf;

  while (true) {
    if ((*dataQueue).size() > 0) {
      string data = (*dataQueue)[0];
      (*dataQueue).erase((*dataQueue).begin());

      // send data to server
      int len = data.length() + 1;
      send(my_data->sock, &len, sizeof(len), 0);
      send(my_data->sock, data.c_str(), len, 0);

      // block on receive
      string titleCasedData;
      recv(my_data->sock, &len, sizeof(len), 0);

      char *msg = new char[len];
      recv(my_data->sock, msg, len, 0);
      
      //Printing the response from the server:
      cout << "Server: " << msg << endl;

      sleep(2);
    }
  }

  pthread_exit(NULL);
}

void connect_to_binder() {
  char *serverAddress = getenv("BINDER_ADDRESS");
  char *port = getenv("BINDER_PORT");

  if (serverAddress == NULL || port == NULL) {
    cerr << "Missing SERVER_ADDRESS or SERVER_PORT environment variable" << endl;
    cerr << *port << endl;
    cerr << *serverAddress << endl;  
    exit(1);
  }

  string data;
  vector<string> buffer;

  int sockfd, portno;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  portno = atoi(port);
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  server = gethostbyname(serverAddress);

  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

  serv_addr.sin_port = htons(portno);

  connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

  struct thread_data td;
  td.sock = sockfd;
  td.buf = &buffer;

  pthread_t thread;
  
  int val = pthread_create(&thread, NULL, SendToServer, (void *)&td);

  binder_sock = sockfd;
}


int rpcRegister(char * name, int *argTypes, skeleton f){

  connect_to_binder();
  int status = RegisterRequestMessage request_message = new RegisterRequestMessage(serverIdentifier, port, name, argTypes);
  success_message.send(sock);
  int retStatus;

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