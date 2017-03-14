CXX = g++
CXXFLAGS = -Wall -MMD -lpthread
AR = ar
ARFLAGS = rcs
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}
SOURCES = $(wildcard *.cc)
TAR = tar
TARFLAGS = cvzpf
TARNAME = a3.tar.gz
TARSOURCES = $(wildcard *.cc) README $(wildcard *.pdf)

OBJECTS1 = binder.o segment.o message.o loc_request_message.o loc_success_message.o loc_failure_message.o execute_request_message.o execute_success_message.o execute_failure_message.o register_request_message.o register_success_message.o register_failure_message.o terminate_message.o constants.o helper_functions.o network.o

EXEC1 = binder

OBJECTS = ${OBJECTS1}
DEPENDS = ${OBJECTS:.o=.d}
EXECS = ${EXEC1}

.PHONY: all clean tar

all: ${EXECS}
	${CXX} -c -lpthread rpc.cc server.c serverA.c serverB.c client.c clientA.c server_functions.c server_function_skels.c
	${AR} ${ARFLAGS} librpc.a rpc.o segment.o message.o loc_request_message.o loc_success_message.o loc_failure_message.o execute_request_message.o execute_success_message.o execute_failure_message.o register_request_message.o register_success_message.o register_failure_message.o terminate_message.o constants.o helper_functions.o network.o

${EXEC1}: ${OBJECTS1}
	${CXX} $^ -o $@

${OBJECTS}: ${MAKEFILE_NAME}

-include ${DEPENDS}

tar:
	${TAR} ${TARFLAGS} ${TARNAME} ${TARSOURCES}

clean:
	rm -f *.d *.o ${EXECS}
