CC=g++ -g -Wall -std=c++17

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC+=-D_XOPEN_SOURCE
    LIBCPU=libcpu_macos.o
else
    LIBCPU=libcpu.o
endif

# List of source files for your thread library
THREAD_SOURCES=thread_lib.cpp mutex.cpp cv.cpp

# Generate the names of the thread library's object files
THREAD_OBJS=${THREAD_SOURCES:.cpp=.o}

all: libthread.o app

# Compile the thread library and tag this compilation
libthread.o: ${THREAD_OBJS}
	./autotag.sh push
	ld -r -o $@ ${THREAD_OBJS}

# Compile an application program
app: app.cpp libthread.o ${LIBCPU}
	${CC} -o $@ $^ -ldl -pthread

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${THREAD_OBJS} libthread.o app
