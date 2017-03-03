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

string do_stuff(){

}

char * get_msg(int sock, vector<int>& myToRemove) {
  int status;
  char * errorMsg = "ERROR";
  int msg_len = 0;

  status = recv(sock, &msg_len, sizeof msg_len, 0);

  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return errorMsg;
  }

  if (status == 0) {
    myToRemove.push_back(sock);
    return errorMsg;
  }

  char* msg = new char[msg_len];
  status = recv(sock, msg, msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return errorMsg;
  }
  return msg;
}

void send_result(int sock, string result) {
  int status;
  int msg_len;
  
  const char* to_send = result.c_str();
  msg_len = strlen(to_send) + 1;
  status = send(sock, &msg_len, sizeof msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return;
  }

  status = send(sock, to_send, msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: send failed" << endl;
    return;
  }
}

int main(){

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

  cout << "SERVER_ADDRESS " << hostname << endl;
  cout << "SERVER_PORT " << ntohs(sin.sin_port) << endl;

  fd_set readfds;
  int n;
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
            char * msg = get_msg(tempConnection, myToRemove);      
            //string str(msg);
            //string result = title_case(str);
            string result = do_stuff();
            send_result(tempConnection, result);
            delete[] msg;

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
