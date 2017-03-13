make clean && 
make && 
g++ -L. client.o -lrpc -lpthread -o client && 
g++ -L. clientA.o -lrpc -lpthread -o clientA && 
g++ -L. server_functions.o server_function_skels.o server.o -lrpc -lpthread -o server && 
g++ -L. server_functions.o server_function_skels.o serverA.o -lrpc -lpthread  -o serverA &&
g++ -L. server_functions.o server_function_skels.o serverB.o -lrpc -lpthread -o serverB
