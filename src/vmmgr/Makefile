BIN 		= vmmgr.out
CC 			= g++

# LINCS 		= 
# LIBS

DEF_INC 	= -I../../lib/MyRmq -I../../lib/librabbitmq \
			-I../../lib

INCS 		= $(DEF_INC)

CFLAGS 		= $(INCS)
CFLAGS 		+= -g -Wall -O2 -std=c++11
LDFLAGS 	= -lrabbitmq

SRC_CPP 	= $(wildcard ./*.cpp ../../lib/MyRmq/*.cpp)

vmmgr.out: vmmgr.cpp
	g++ vmmgr.cpp ../../lib/MyRmq/*.cpp 

all: $(BIN)

$(BIN) : $(SRC_CPP)
	$(CC) -o $(BIN) $(SRC_CPP) $(CFLAGS) $(LDFLAGS)