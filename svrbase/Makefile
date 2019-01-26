LIB_NAME ?= svrbase
CC = g++
AR	= ar
FLAG = -std=gnu++0x -fPIC
INCLUDE += -I SvrBase/base/include	\
		-I SvrBase/cfg/include	\
		-I SvrBase/libuv/include \
		-I third/libuv/linux/include	\
		-I third/rapidjson
LIBDIR =
LIB = -lpthread 
INSTALL_DIR = /usr/lib/mylib
DIRS = $(shell find $(shell pwd) -maxdepth 3 -type d)  
SRCS += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))  

all:static_library shared_library
	#cp -f lib${LIB_NAME}.a lib${LIB_NAME}.so ${INSTALL_DIR}/lib
	#cp -f include/* ${INSTALL_DIR}/include/${LIB_NAME}

static_library:$(SRCS:.cpp=.o)
	$(AR) -cr  lib${LIB_NAME}.a $^;
	
shared_library:$(SRCS:.cpp=.o)
	$(CC) -shared -fpic -o lib${LIB_NAME}.so $^;
	
%.o:%.cpp
	$(CC) $(FLAG) $(INCLUDE) $< -c -o $@

clean:
	find $(shell pwd) -name "*\.o" -o -name "*\.d" -o -name "*\.a" -o -name "*\.so"| xargs rm -rf
