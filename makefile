CXX = g++
CXXFLAGS = -g -Wall 
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	
SOURCES = $(wildcard *.cc)

OBJECTS1 = binder.o segment.o message.o loc_request_message.o  loc_success_message.o loc_failure_message.o execute_request_message.o execute_success_message.o execute_failure_message.o register_request_message.o register_success_message.o register_failure_message.o terminate_message.o constants.o helper_function.o network.o

EXEC1 = binder				

OBJECTS = ${OBJECTS1}
DEPENDS = ${OBJECTS:.o=.d}			
EXECS = ${EXEC1}


.PHONY : all clean

all : ${EXECS}					
	${CXX} -c rpc.c server.c client.c server_functions.c server_function_skels.c
	ar rcs librpc.a  rpc.o segment.o message.o loc_request_message.o loc_success_message.o loc_failure_message.o execute_request_message.o execute_success_message.o execute_failure_message.o register_request_message.o register_success_message.o register_failure_message.o terminate_message.o constants.o helper_function.o network.o


${EXEC1} : ${OBJECTS1}				
	${CXX} $^ -o $@

${OBJECTS} : ${MAKEFILE_NAME}			

-include ${DEPENDS}				

clean :						
	rm -f *.d *.o ${EXECS}

