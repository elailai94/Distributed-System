#ifndef __HELPER_FUNCTION__
#define __HELPER_FUNCTION__

int createConnection(std::string address, unsigned int port);
bool compareArr(const int *array1, const int *array2);
int findLenOfArgTypes(int *array);
int *copyArgTypes(int argTypes[]);

#endif
