CXX = g++
CXXFLAGS = -Wall -MMD -lpthread
AR = ar
ARFLAGS = rcs
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}
SOURCES = $(wildcard *.cc)
BINDEROBJECTS = \
binder.o segment.o message.o loc_request_message.o loc_success_message.o \
loc_failure_message.o execute_request_message.o execute_success_message.o \
execute_failure_message.o register_request_message.o register_success_message.o \
register_failure_message.o terminate_message.o constants.o helper_functions.o \
network.o
BINDEREXEC = binder
RPCOBJECTS = \
rpc.o segment.o message.o loc_request_message.o loc_success_message.o \
loc_failure_message.o execute_request_message.o execute_success_message.o \
execute_failure_message.o register_request_message.o register_success_message.o \
register_failure_message.o terminate_message.o constants.o helper_functions.o \
network.o
RPCLIB = librpc.a
OBJECTS = ${BINDEROBJECTS}
DEPENDS = ${OBJECTS:.o=.d}
EXECS = ${BINDEREXEC} ${RPCLIB}
TAR = tar
TARFLAGS = cvzpf
TARNAME = a3.tar.gz
TAREXCLUDEDSOURCES = server_function_skels.h server_functions.h
TARINCLUDEDSOURCES = $(filter-out $(TAREXCLUDEDSOURCES), $(wildcard *.h)) \
                     $(wildcard *.cc) README $(wildcard *.pdf)

.PHONY: all clean tar

all: ${EXECS}
	${CXX} -c -lpthread rpc.cc server.c serverA.c serverB.c client.c clientA.c server_functions.c server_function_skels.c

${BINDEREXEC}: ${BINDEROBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@

${RPCLIB}: ${RPCOBJECTS}
	${AR} ${ARFLAGS} $@ $^

${OBJECTS}: ${MAKEFILE_NAME}

-include ${DEPENDS}

tar:
	${TAR} ${TARFLAGS} ${TARNAME} ${TARINCLUDEDSOURCES}

clean:
	find . -name "*.o" -delete
	find . -name "*.d" -delete
	find . -name ${BINDEREXEC} -delete
	find . -name ${RPCLIB} -delete
	find . -name ${TARNAME} -delete
