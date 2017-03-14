#ifndef __HELPER_FUNCTIONS__
#define __HELPER_FUNCTIONS__

#include <string>

/*************** Argument types utility functions **************/
// Compares two arrays to see if they have the same elements
bool compareArr(const int *array1, const int *array2);

// Returns a count of the number of argument types
unsigned int countNumOfArgTypes(int* argTypes);

// Returns a copy of argument types
int *copyArgTypes(int *argTypes);

/****************** Type conversion functions ******************/
// Converts an unsigned integer to a string
std::string toString(unsigned int number);

// Converts a string to an integer
int toInt(std::string text);

// Converts a character array to an unsigned integer
unsigned int toUnsignedInt(char *array);

// Converts a character array to a short
short toShort(char *array);

// Converts a character array to an integer
int toInt(char *array);

// Converts a character array to a long
long toLong(char *array);

// Converts a character array to a double
double toDouble(char *array);

// Converts a character array to a float
float toFloat(char *array);

#endif
