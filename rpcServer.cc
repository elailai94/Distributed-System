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

using namespace std;

int rpcInit(){
  	
	//TODO:
  // CREATE SERVER
  // CONNECT TO BINDER
}

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
      //The send() call may be used only when the socket is in a connected state 

      send(my_data->sock, data.c_str(), len, 0);

      // block on receive
      string titleCasedData;
      recv(my_data->sock, &len, sizeof(len), 0);
      // The recv() call is normally used only on a connected socket 
      // Recv() calls are used to receive messages from a socket, and may be used to receive 
      // data on a socket whether or not it is connection-oriented.

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
  char *serverAddress = getenv("SERVER_ADDRESS");
  char *port = getenv("SERVER_PORT");

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
  //The bzero() function sets the first  sizeof(serv_addr) bytes of the area starting at &serv_addr

  serv_addr.sin_family = AF_INET;
  
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  //The bcopy() function copies  server->h_length bytes from server->h_addr to serv_addr.sin_addr.s_addr.  The result is correct, even when both areas overlap.

  serv_addr.sin_port = htons(portno);

  connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
  //The connect() system call connects the socket referred to by the file descriptor sockfd to the address specified by &serv_addr. 
  //The sizeof(serv_addr) argument specifies the size of addr.

  struct thread_data td;
  td.sock = sockfd;
  td.buf = &buffer;

  pthread_t thread;
  
  int val = pthread_create(&thread, NULL, SendToServer, (void *)&td);
  //int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);

  while (true) {
    getline(cin, data);
    buffer.push_back(data);
  }
}

