#include <string>

using namespace std;

void *SendToServer(void *threadarg);
int createConnection(string address, unsigned int port);
bool compareArr(const int* array1, const int* array2);
int findLenOfArgTypes(int * array);
int *copyArgTypes(int argTypes[]);

