CXX = g++
CXXFLAGS = -Wall -MMD
AR = ar
ARFLAGS = rcs
BINDEROBJECTS = \
binder.o segment.o message.o loc_request_message.o loc_success_message.o \
loc_failure_message.o execute_request_message.o execute_success_message.o \
execute_failure_message.o register_request_message.o register_success_message.o \
register_failure_message.o terminate_message.o constants.o helper_functions.o \
network.o
BINDEREXEC = binder
SERVEROBJECTS = server.o server_functions.o server_function_skels.o
SERVEREXEC = server
CLIENTOBJECTS = client.o
CLIENTEXEC = client
RPCOBJECTS = \
rpc.o segment.o message.o loc_request_message.o loc_success_message.o \
loc_failure_message.o execute_request_message.o execute_success_message.o \
execute_failure_message.o register_request_message.o register_success_message.o \
register_failure_message.o terminate_message.o constants.o helper_functions.o \
network.o
RPCLIB = librpc.a
OBJECTS = ${BINDEROBJECTS} ${SERVEROBJECTS} ${CLIENTOBJECTS}
DEPENDS = ${OBJECTS:.o=.d}
EXECS = ${BINDEREXEC} ${SERVEREXEC} ${CLIENTEXEC}
TAR = tar
TARFLAGS = cvzpf
TARNAME = a3.tar.gz
TAREXCLUDEDSOURCES = server_function_skels.h server_functions.h
TARINCLUDEDSOURCES = $(filter-out $(TAREXCLUDEDSOURCES), $(wildcard *.h)) \
                     $(wildcard *.cc) makefile README $(wildcard *.pdf)

.PHONY: all clean tar

all: ${RPCLIB} ${BINDEREXEC}

test: ${RPCLIB} ${EXECS}

${BINDEREXEC}: ${BINDEROBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@

${SERVEREXEC}: ${SERVEROBJECTS}
	${CXX} ${CXXFLAGS} -L. $^ -lrpc -lpthread -o $@

${CLIENTEXEC}: ${CLIENTOBJECTS}
	${CXX} ${CXXFLAGS} -L. $^ -lrpc -lpthread -o $@

${RPCLIB}: ${RPCOBJECTS}
	${AR} ${ARFLAGS} $@ $^

-include ${DEPENDS}

runBinder: test
	./binder

tar:
	${TAR} ${TARFLAGS} ${TARNAME} ${TARINCLUDEDSOURCES}

clean:
	find . -name "*.o" -delete
	find . -name "*.d" -delete
	find . -name ${CLIENTEXEC} -delete
	find . -name ${SERVEREXEC} -delete
	find . -name ${BINDEREXEC} -delete
	find . -name ${RPCLIB} -delete
	find . -name ${TARNAME} -delete
