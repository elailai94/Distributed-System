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

#include <netdb.h>
#include <stdlib.h>
using namespace std;

struct thread_data {
  int sock;
  vector<string> *buf;
};

void *SendToServer(void *threadarg);
int createConnection(char * address, char * port);
bool compareArr(const int* array1, const int* array2);
int findLenOfArgTypes(int * array);
char * extractUnsignedInt(char * bufferP, int &i);
int *copyArgTypes(int argTypes[]);