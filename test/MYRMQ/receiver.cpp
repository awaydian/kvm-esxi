#include "MyRmq.h"

#include <iostream>

char * process(char * request)
{
	char * resp_str = "response...";
	char * response = (char*)malloc((int)strlen(resp_str) + 1);
	strcpy(response,resp_str);

	std::cout << "request: " << request << std::endl;
	std::cout << "response: " << response << std::endl;

	return response;
}


int main()
{
	Callback p = NULL;
	char * resp = NULL;
	p = process;
	MyRmq MyRmq("guest","guest","0.0.0.0",5672);
	MyRmq.receive("amq.direct","my_binding_key","my_queue",p);

	return 0;
}

