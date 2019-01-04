#ifndef my_MyRmq
#define my_MyRmq

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <amqp.h>
#include <amqp_tcp_socket.h>

#include <string>
#include <iostream>

#include "utils.h"

typedef char* (*Callback)(char*);


class MyRmq
{
public:
	MyRmq(std::string user,std::string pwd,std::string ip,int port);
	~MyRmq();
	void receive(std::string exchange,std::string binding_key,std::string queue_name,Callback fun);
	char* send(std::string exchange,std::string routing_key,std::string data);
private:
	amqp_socket_t * socket;
	amqp_connection_state_t conn;
};
#endif