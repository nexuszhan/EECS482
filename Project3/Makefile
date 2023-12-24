CC=g++ -g -Wall -fno-builtin -std=c++17

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC+=-D_XOPEN_SOURCE
    LIBVMAPP=libvm_app_macos.o
    LIBVMPAGER=libvm_pager_macos.o
else
    LIBVMAPP=libvm_app.o
    LIBVMPAGER=libvm_pager.o
endif

# List of source files for your pager
PAGER_SOURCES= my_vm_pager.cpp my_utils.cpp

# Generate the names of the pager's object files
PAGER_OBJS=${PAGER_SOURCES:.cpp=.o}

all: pager app

# Compile the pager and tag this compilation
pager: ${PAGER_OBJS} ${LIBVMPAGER}
#	./autotag.sh push
	${CC} -o $@ $^

# Compile an application program
app: app.cpp ${LIBVMAPP}
	${CC} -o $@ $^ -ldl

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${PAGER_OBJS} pager test1
