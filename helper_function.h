#ifdef __HELPER_FUNCTION_H__
#define __HELPER_FUNCTION_H__

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

#include "constants.h"

using namespace std;

void *SendToServer(void *threadarg);
int createConnection(string address, unsigned int port);
bool compareArr(const int* array1, const int* array2);
int findLenOfArgTypes(int * array);
char * extractUnsignedInt(char * bufferP, int &i);
int *copyArgTypes(int argTypes[]);

#endif
