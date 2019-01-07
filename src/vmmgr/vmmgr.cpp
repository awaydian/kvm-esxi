#include "MyRmq.h"

#include <iostream>


#include "rapidjson/document.h"


char * process(char * request)
{
	rapidjson::Document doc;
	doc.Parse(request);
	// if (!doc.HasMember("msg_type") || !doc["msg_type"].IsInt())
	// {
	// 	return "{}";
	// }
	assert(doc.HasMember("msg_type"));
	assert(doc["msg_type"].IsInt());

	int msg_type = doc["msg_type"].GetInt();
	std::cout << "msg_type: " << msg_type << std::endl;

	char * resp_str = NULL;

	switch (msg_type) {
		case 200:
			resp_str = "vm list";
			break;
		default:
			break;
	}

	char * response = (char*)malloc((int)strlen(resp_str) + 1);
	strcpy(response,resp_str);

	return response;
}


int main()
{
	MyRmq MyRmq("guest","guest","0.0.0.0",5672);
	MyRmq.receive("amq.direct","my_binding_key","my_queue",process);

	// const char * json = "{\"msg_type\": 200}";
	//// {"msg_type": 200}
	
	// rapidjson::Document doc;
	// doc.Parse(json);
	// assert(doc.HasMember("msg_type"));
	// assert(doc["msg_type"].IsInt());
	// std::cout << doc["msg_type"].GetInt() <<std::endl;

	return 0;
}

