#include "server_functions.h"
#include <create_server.cpp>

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

using namespace std;

//TODO:
//MANAGE CLIENT CONNECTION
//MANAGE SERVER CONNECTION
//DATABASE
//ROUND ROBIN


char * get_msg(int sock, vector<int>& myToRemove) {
  int status;
  char * errorMsg = "ERROR";

  // receive the buffer length
  int msg_len = 0;
  status = recv(sock, &msg_len, sizeof msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return errorMsg;
  }
  if (status == 0) {
    // client has closed the connection
    myToRemove.push_back(sock);
    return errorMsg;
  }

  // receive the string
  char* msg = new char[msg_len];
  status = recv(sock, msg, msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return errorMsg;
  }
  return msg;
}

void send_result(int sock, vector<int>& myToRemove, string result) {
  int status;

  // send the buffer length
  const char* to_send = result.c_str();
  msg_len = strlen(to_send) + 1;
  status = send(sock, &msg_len, sizeof msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return;
  }

  // send the result string
  status = send(sock, to_send, msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return;
  }

  delete[] msg;
}



int main(){

	//TODO:
	//MANAGE CLIENT CONNECTION
	//MANAGE SERVER CONNECTION
	//DATABASE
	//ROUND ROBIN


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

  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 0;
  }

  p = servinfo;
  int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
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
  const char* portString = ps.c_str();

  cout << "SERVER_ADDRESS " << hostname << endl;
  cout << "SERVER_PORT " << ntohs(sin.sin_port) << endl;

  fd_set readfds;
  int n;
  struct sockaddr_storage their_addr;

  while(true){
    //CONNECTIONS VECTOR
    
    FD_ZERO(&readfds); //This macro initializes the file descriptor set &readfds to be the empty set.
    FD_SET(sock, &readfds); //This macro adds sock to the file descriptor set &readfds.

    n = sock; //set variable n to be the file descriptor

    for (vector<int>::iterator it = myConnections.begin();it != myConnections.end(); ++it) {
      int connection = *it;
      FD_SET(connection, &readfds);
      //This macro adds connection to the file descriptor set &readfds.
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
 
            char * msg = get_msg(tempConnection, myToRemove);
			
              // We need to do stuff here, 
			  // msg is the input
			  // string str(msg);
  		 	  // string result = title_case(str);

			send_result(tempConnection, myToRemove, result);

          }
        }
      }

      for (vector<int>::iterator it = myToRemove.begin();
          it != myToRemove.end(); ++it) {
        myConnections.erase(remove(myConnections.begin(), myConnections.end(), *it), myConnections.end());
        close(*it);
      }
      myToRemove.clear();
    }
  }

  freeaddrinfo(servinfo);
}
