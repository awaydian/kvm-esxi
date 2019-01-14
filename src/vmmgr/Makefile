BIN 		= vmmgr.out
CC 			= g++

# LINCS 		= 
# LIBS

DEF_INC 	= -I../../lib/MyRmq -I../../lib/librabbitmq \
			-I../../lib -I./

INCS 		= $(DEF_INC)

CFLAGS 		= $(INCS)
CFLAGS 		+= -g -Wall -O2 -std=c++11
LDFLAGS 	= -lrabbitmq -lvirt

SRC_CPP 	= $(wildcard ./*.cpp ./virsh/*.cpp ../../lib/MyRmq/*.cpp ../../lib/common/*.cpp)

TEST_CPP 	= $(wildcard ./virsh/*.cpp ../../lib/MyRmq/*.cpp ../../lib/common/*.cpp)

all: $(BIN) test.out

$(BIN) : $(SRC_CPP)
	$(CC) -o $(BIN) $(SRC_CPP) $(CFLAGS) $(LDFLAGS)

test.out : $(SRC_CPP) test.c
	$(CC) -o test.out $(TEST_CPP) test.c $(CFLAGS) $(LDFLAGS)
