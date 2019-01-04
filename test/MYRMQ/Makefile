all: receiver.out sender.out


receiver.out: receiver.cpp MyRmq.cpp
	g++ -std=c++11 receiver.cpp MyRmq.cpp utils.c -o receiver.out -w -g -lrabbitmq

sender.out: sender.cpp MyRmq.cpp
	g++ -std=c++11 sender.cpp MyRmq.cpp utils.c -o sender.out -w -g -lrabbitmq