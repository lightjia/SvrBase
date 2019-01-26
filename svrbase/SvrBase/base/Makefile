LIB_NAME ?= base
CC = g++
AR	= ar
FLAG = -std=gnu++0x -fPIC
INCLUDE += -I include
LIBDIR =
LIB = -lpthread 
INSTALL_DIR = /usr/local/lib/mylib
DIRS = $(shell find $(shell pwd) -maxdepth 3 -type d)  
SRCS += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))  

all:static_library shared_library
	cp -f lib${LIB_NAME}.a ${INSTALL_DIR}/static
	cp -f lib${LIB_NAME}.so ${INSTALL_DIR}/dynamic
	cp -f include/* ${INSTALL_DIR}/include/${LIB_NAME}

static_library:$(SRCS:.cpp=.o)
	$(AR) -cr  lib${LIB_NAME}.a $^;
	
shared_library:$(SRCS:.cpp=.o)
	$(CC) -shared -fpic -o lib${LIB_NAME}.so $^;
	
%.o:%.cpp
	$(CC) $(FLAG) $(INCLUDE) $< -c -o $@

clean:
	find $(shell pwd) -name "*\.o" -o -name "*\.d" -o -name "*\.a" -o -name "*\.so"| xargs rm -rf

