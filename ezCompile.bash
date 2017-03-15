make clean &&
make test &&
g++ -c -lrpc -lpthread serverA.c serverB.c clientA.c server_functions.c server_function_skels.c
g++ -L. clientA.o -lrpc -lpthread -o clientA &&
g++ -L. server_functions.o server_function_skels.o serverA.o -lrpc -lpthread  -o serverA &&
g++ -L. server_functions.o server_function_skels.o serverB.o -lrpc -lpthread -o serverB
