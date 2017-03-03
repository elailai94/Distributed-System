#include <iostream>
#include <string>
#include <sstream>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;


void process_connection(int sock, vector<int>& myToRemove) {
  int status;

  // receive the buffer length
  int msg_len = 0;
  status = recv(sock, &msg_len, sizeof msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return;
  }
  if (status == 0) {
    // client has closed the connection
    myToRemove.push_back(sock);
    return;
  }

  // receive the string
  char* msg = new char[msg_len];
  status = recv(sock, msg, msg_len, 0);
  if (status < 0) {
    cerr << "ERROR: receive failed" << endl;
    return;
  }

  /* 
    HERE IS WHERE WE DO STUFF
  */

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



void create_server(){

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
  //http://man7.org/linux/man-pages/man3/getaddrinfo.3.html

  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return 0;
  }

  p = servinfo;
  int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
  //creates an endpoint for communication and returns a file descriptor that refers to that endpoint

  status = bind(sock, servinfo->ai_addr, servinfo->ai_addrlen);
  // bind - bind a name to a socket
  // bind() assigns  the address specified by servinfo->ai_addr to the socket referred
  // to by the file descriptor sock.  p->ai_protocol specifies the size, in bytes, of the
  // address structure pointed to by servinfo->ai_addr.  

 
  status = listen(sock, 5);
  // listen() marks the socket referred to by sock as a passive socket,
  //that is, as a socket that will be used to accept incoming connection
  //requests using accept(2).

  //The backlog (5 in our case) argument defines the maximum length to which the queue of
  //pending connections for sockfd may grow.

  char hostname[256];
  gethostname(hostname, 256);
  //http://man7.org/linux/man-pages/man2/sethostname.2.html

  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);

  getsockname(sock, (struct sockaddr *)&sin, &len);
  //http://man7.org/linux/man-pages/man2/getsockname.2.html

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

    //int select (int nfds, 
    //            fd_set *read-fds, 
    //            fd_set *write-fds, 
    //            fd_set *except-fds, 
    //            struct timeval *timeout)  

    status = select(n, &readfds, NULL, NULL, NULL);
    
    //The select function blocks the calling process until there is activity on any of the specified sets of file descriptors, or until the timeout period has expired.
    //The file descriptors specified by the read-fds argument are checked to see if they are ready for reading; 
  

    if (status == -1) {
      cerr << "ERROR: select failed." << endl;
    } else {
      
      if (FD_ISSET(sock, &readfds)) { 
      // This macro returns a nonzero value (true) if sock is a member of the file descriptor set &readfds, and zero (false) otherwise.
      
        socklen_t addr_size = sizeof their_addr;
        int new_sock = accept(sock, (struct sockaddr*)&their_addr, &addr_size);

        if (new_sock < 0) {
          cerr << "ERROR: while accepting connection" << endl;
          close(new_sock);
          continue;
        }

        //Adding a new connection
        myConnections.push_back(new_sock);

      } else { //FD_ISSET is false meaning sock is not a member of file descriptor
          
        for (vector<int>::iterator it = myConnections.begin(); it != myConnections.end(); ++it) {   
          int tempConnection = *it;
          
          if (FD_ISSET(tempConnection, &readfds)) {
          // This macro returns a nonzero value (true) if connection is a member of the file descriptor set &readfds, and zero (false) otherwise.
    
            process_connection(tempConnection, myToRemove);
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

  // free the linked list
  freeaddrinfo(servinfo);
  //Do we need this TODO

}
