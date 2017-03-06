CXX = g++
CXXFLAGS = -Wall -MMD
AR = ar
ARFLAGS = rcs
CLIENTOBJECTS = client.o
SERVEROBJECTS = server.o server_functions.o server_function_skels.o
BINDEROBJECTS = binder.o segment.o message.o register_request_message.o register_success_message.o register_failure_message.o loc_request_message.o loc_success_message.o loc_failure_message.o constants.o helper_function.o message_types.o message_results.o
RPCOBJECTS = rpc.o segment.o message.o loc_request_message.o loc_success_message.o loc_failure_message.o execute_request_message.o execute_success_message.o execute_failure_message.o register_request_message.o register_success_message.o register_failure_message.o constants.o helper_function.o message_types.o message_results.o
##OBJECTS = ${BINDEROBJECTS}
DEPENDS = ${BINDEROBJECTS: .o=.d}
RPCEXEC = librpc.a
BINDEREXEC = binder
CLIENTEXEC = client
SERVEREXEC = server
EXECS = ${BINDEREXEC}

.PHONY: all clean

all:
	@echo "Compiling..."
	${BINDEREXEC}
	@echo "Granting file execution permissions..."
	chmod u+x ${EXECS}

${RPCEXEC}: ${RPCOBJECTS}
	${AR} ${ARFLAGS} ${RPCEXEC} ${RPCOBJECTS}

${BINDEREXEC}: ${BINDEROBJECTS}
	${CXX} ${CXXFLAGS} ${BINDEROBJECTS} -o ${BINDEREXEC}

${CLIENTEXEC}: ${CLIENTOBJECTS}
	${CXX} ${CXXFLAGS} ${CLIENTOBJECTS} -o ${CLIENTEXEC}

${SERVEREXEC}: ${SERVEROBJECTS}
	${CXX} ${CXXFLAGS} ${SERVEROBJECTS} -o ${SERVEREXEC}

-include ${DEPENDS}

clean:
	find . -name ${BINDEREXEC} -delete
	find . -name ${BINDEROBJECTS} -delete
	find . -name ${DEPENDS} -delete
