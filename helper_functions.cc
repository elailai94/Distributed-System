#include <sstream>

#include "rpc.h"
#include "helper_functions.h"
#include "constants.h"

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

// See interface (header file).
string toString(unsigned int number) {
  stringstream ss;
  ss << number;
  return ss.str();
}

// See interface (header file).
unsigned int toUnsignedInteger(string text) {
  stringstream ss;
  ss << text;

  unsigned int number = 0;
  ss >> number;

  return number;
}
