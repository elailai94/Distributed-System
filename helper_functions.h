#ifndef __HELPER_FUNCTIONS__
#define __HELPER_FUNCTIONS__

#include <string>

bool compareArr(const int *array1, const int *array2);

// Returns a count of the number of argument types
unsigned int countNumOfArgTypes(int* argTypes);

int *copyArgTypes(int argTypes[]);

// Converts an unsigned number to a string
std::string toString(unsigned int number);

// Converts a string to an unsigned number
unsigned int toUnsignedInteger(std::string text);

#endif
