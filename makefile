CXX = g++
CXXFLAGS = -Wall -MMD
AR = ar
ARFLAGS = acs
CLIENTOBJECTS = client.o
SERVEROBJECTS = server.o
BINDEROBJECTS = binder.o
RPCOBJECTS = rpc.o
OBJECTS = ${CLIENTOBJECTS} ${SERVEROBJECTS} ${BINDEROBJECTS} ${RPCOBJECTS}
DEPENDS = ${OBJECTS: .o=.d}
CLIENTEXEC = client
SERVEREXEC = server
BINDEREXEC = binder
RPCEXEC = librpc.a
EXECS = ${CLIENTEXEC} ${SERVEREXEC} ${BINDEREXEC} ${RPCEXEC}

${CLIENTEXEC}: ${CLIENTOBJECTS}
	${CXX} ${CXXFLAGS} ${CLIENTOBJECTS} -o ${CLIENTEXEC}

${SERVEREXEC}: ${SERVEROBJECTS}
	${CXX} ${CXXFLAGS} ${SERVEROBJECTS} -o ${SERVEREXEC}

${BINDEREXEC}: ${BINDEROBJECTS}
	${CXX} ${CXXFLAGS} ${BINDEROBJECTS} -o ${BINDEREXEC}

${RPCEXEC}: ${RPCOBJECTS}
	${AR} ${ARFLAGS} ${RPCEXEC} ${RPCOBJECTS}

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
