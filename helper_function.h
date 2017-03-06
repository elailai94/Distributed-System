#ifndef __HELPER_FUNCTION__
#define __HELPER_FUNCTION__

#include <string>

using namespace std;

int createConnection(string address, unsigned int port);
bool compareArr(const int* array1, const int* array2);
int findLenOfArgTypes(int * array);
int *copyArgTypes(int argTypes[]);

#endif
