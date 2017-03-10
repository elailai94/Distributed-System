#include <sstream>

#include "helper_functions.h"

using namespace std;

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

// See interface (header file).
unsigned int countNumOfArgTypes(int *argTypes) {
  unsigned int count = 1;
  while (argTypes[(count - 1)] != 0) {
    count += 1;
  }

  return count;
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

// See interface (header file).
string toString(unsigned int number) {
  stringstream ss;
  ss << number;
  return ss.str();
}

// See interface (header file).
unsigned int toUnsignedInt(string text) {
  stringstream ss;
  ss << text;

  unsigned int number = 0;
  ss >> number;

  return number;
}

// See interface (header file).
unsigned int toUnsignedInt(char *array) {
  return *((unsigned int *) array);
}

// See interface (header file).
short toShort(char *array) {
  return *((short *) array);
}

// See interface (header file).
int toInt(char *array) {
  return *((int *) array);
}

// See interface (header file).
long toLong(char *array) {
  return *((long *) array);
}

// See interface (header file).
double toDouble(char *array) {
  return *((double *) array);
}

// See interface (header file).
float toFloat(char *array) {
  return *((float *) array);
}
