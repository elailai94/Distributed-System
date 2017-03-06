CXX = g++
CXXFLAGS = -Wall -MMD
AR = ar
ARFLAGS = rcs

RPCOBJECTS = rpc.o segment.o message.o loc_request_message.o \
loc_success_message.o loc_failure_message.o execute_request_message.o \
execute_success_message.o execute_failure_message.o register_request_message.o \
register_success_message.o register_failure_message.o terminate_message.o \
constants.o helper_function.o
RPCEXEC = librpc.a

BINDEROBJECTS = binder.o segment.o message.o loc_request_message.o \
loc_success_message.o loc_failure_message.o execute_request_message.o \
execute_success_message.o execute_failure_message.o register_request_message.o \
register_success_message.o register_failure_message.o terminate_message.o \
constants.o helper_function.o
BINDEREXEC = binder

CLIENTOBJECTS = client.o
CLIENTEXEC = client

SERVEROBJECTS = server.o server_functions.o server_function_skels.o
SERVEREXEC = server

OBJECTS = ${BINDEROBJECTS}
DEPENDS = ${OBJECTS: .o=.d}
EXECS = ${BINDEREXEC}

.PHONY: all clean

all:
	@echo "Compiling..."
	${EXECS}
	@echo "Granting file execution permissions..."
	chmod u+x ${EXECS}

${RPCEXEC}: ${RPCOBJECTS}
	${AR} ${ARFLAGS} $@ $^

${BINDEREXEC}: ${BINDEROBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@

${CLIENTEXEC}: ${CLIENTOBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@

${SERVEREXEC}: ${SERVEROBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@

-include ${DEPENDS}

clean:
	find . -name ${EXECS} -delete
	find . -name ${OBJECTS} -delete
	find . -name ${DEPENDS} -delete
