#ifndef __HELPER_FUNCTIONS__
#define __HELPER_FUNCTIONS__

#include <string>

int createConnection(std::string address, unsigned int port);
bool compareArr(const int *array1, const int *array2);
int findLenOfArgTypes(int *array);
int *copyArgTypes(int argTypes[]);

// Converts an unsigned number to a string
std::string toString(unsigned int number);

// Converts a string to an unsigned number
unsigned int toUnsignedInteger(std::string text);

#endif
