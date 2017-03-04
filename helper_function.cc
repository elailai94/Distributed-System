#include <iostream>
#include <string>
#include <list>
#include <algorithm>
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
