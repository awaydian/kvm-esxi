#include "MyRmq.h"

#include <iostream>

int main()
{
	char * resp = NULL;
	MyRmq MyRmq("guest","guest","0.0.0.0",5672);
	// MyRmq.listen("amq.direct","my_binding_key","my_queue",process);
	resp = MyRmq.send("amq.direct","my_binding_key","request...");
	if (resp != NULL){
		std::cout << "receive response: " << resp << std::endl;
		free(resp);
	}

	return 0;
}

