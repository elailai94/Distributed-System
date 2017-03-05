#include <stdio.h>
#include <string.h>
#include "rpc.h"

#include <iostream>
#include <sstream>

#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <algorithm>
#include <string>

#include <netdb.h>
#include <stdlib.h>

#include "helper_function.h"
#include "constants.h"

using namespace std;


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


int createConnection(string address, unsigned int port){

  string data;
  vector<string> buffer;

  int sockfd, portno;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  portno = port;
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  server = gethostbyname(address.c_str());

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

  serv_addr.sin_port = htons(portno);

  connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));
  struct thread_data td;
  td.sock = sockfd;
  td.buf = &buffer;

  pthread_t thread;

  pthread_create(&thread, NULL, SendToServer, (void *)&td);

  return sockfd;
}


bool compareArr(const int* array1, const int* array2){
  while (*array1 != 0 || *array2 != 0){
      if (*array1 != *array2){
          return false;
      }
      *array1++;
      *array2++;
  }

  return true;
}

int findLenOfArgTypes(int * array){
  unsigned int counter = 0;
  while(true){
    counter++;

    int type = *array;
    array++;
    if(type == 0){
      break;
    }
  }
}

int *copyArgTypes(int *argTypes) {
    unsigned int argTypesLength = 0;
    
    while (argTypes[argTypesLength]){
      argTypesLength++;
    }

    int *memArgTypes = new int[argTypesLength];

    unsigned int i = 0;
    while (argTypes[i] != 0) {
        memArgTypes[i] = argTypes[i];
        i++;
    }
    memArgTypes[i] = 0;

    return memArgTypes;
}

