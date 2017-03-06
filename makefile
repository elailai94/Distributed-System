CXX = g++
CXXFLAGS = -Wall -MMD
AR = ar
ARFLAGS = acs
CLIENTOBJECTS = client.o
SERVEROBJECTS = server.o server_functions.o server_function_skels.o
BINDEROBJECTS = binder.o segment.o message.o register_request_message.o \
register_success_message.o register_failure_message.o loc_request_message.o \
loc_success_message.o loc_failure_message.o terminate.o
RPCOBJECTS = rpc.o segment.o message.o loc_request_message.o \
loc_success_message.o loc_failure_message.o execute_request_message.o \
execute_success_message.o execute_failure_message.o register_request_message.o \
register_success_message.o register_failure_message.o terminate.o constants.o \
helper_function.o message_types.o message_results.o
OBJECTS = ${RPCOBJECTS} ${BINDEROBJECTS}
DEPENDS = ${OBJECTS: .o=.d}
RPCEXEC = librpc.a
BINDEREXEC = binder
CLIENTEXEC = client
SERVEREXEC = server
EXECS = ${RPCEXEC} ${BINDEREXEC}

${RPCEXEC}: ${RPCOBJECTS}
	${AR} ${ARFLAGS} ${RPCEXEC} ${RPCOBJECTS}

${BINDEREXEC}: ${BINDEROBJECTS}
	${CXX} ${CXXFLAGS} ${BINDEROBJECTS} -o ${BINDEREXEC}

${CLIENTEXEC}: ${CLIENTOBJECTS}
	${CXX} ${CXXFLAGS} ${CLIENTOBJECTS} -o ${CLIENTEXEC}

${SERVEREXEC}: ${SERVEROBJECTS}
	${CXX} ${CXXFLAGS} ${SERVEROBJECTS} -o ${SERVEREXEC}

-include ${DEPENDS}

.PHONY: all clean

all:
	@echo "Compiling..."
	${EXECS}
	@echo "Granting file execution permissions..."
	chmod u+x ${EXECS}

clean:
	find . -name ${EXECS} -delete
	find . -name ${OBJECTS} -delete
	find . -name ${DEPENDS} -delete
