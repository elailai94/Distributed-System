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

      int len = data.length() + 1;
      send(my_data->sock, &len, sizeof(len), 0);
      send(my_data->sock, data.c_str(), len, 0);

      string titleCasedData;
      recv(my_data->sock, &len, sizeof(len), 0);
      char *msg = new char[len];
      recv(my_data->sock, msg, len, 0);
      
      cout << "Server: " << msg << endl;

      sleep(2);
    }
  }

  pthread_exit(NULL);
}

int  send_LOC_REQUEST(string name, int argTypes[]){
    int status = connect_Binder();

    if (status == 0) {
        Sender s(binderSocketFd);
        status = s.sendLocRequestMessage(name, argTypes);
    }

    return status;
}



int connect_Binder() {
    char * binderAddressString = getenv ("BINDER_ADDRESS");
    char * binderPortString = getenv("BINDER_PORT");

    if(binderAddressString == NULL){

        return INIT_UNSET_BINDER_ADDRESS;
    }

    if(binderPortString == NULL){
        return INIT_UNSET_BINDER_PORT;
    }
    
    //binderSocketFd = setupSocketAndReturnDescriptor(binderAddressString, binderPortString);

    if (binderSocketFd < 0) {
        return INIT_BINDER_SOCKET_FAILURE;
    }
    return 0;
}

int main() {
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
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

  serv_addr.sin_port = htons(portno);

  connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
  struct thread_data td;
  td.sock = sockfd;
  td.buf = &buffer;

  pthread_t thread;
  
  val = pthread_create(&thread, NULL, SendToServer, (void *)&td);

  while (true) {
    getline(cin, data);
    buffer.push_back(data);
  }


}


