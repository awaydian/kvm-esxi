#include "MyRmq.h"
#include "virsh/Virsh.h"

#include <iostream>
#include <vector>
#include <map>


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


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
			{
				resp_str = "vm list";
				std::vector<std::map<std::string,std::string> > vmList;
				Virsh vir("qemu+ssh://root@192.168.2.236/system?socket=/var/run/libvirt/libvirt-sock");
				if( vir.getVmList(vmList)) {
					std::cout << "getVmList error\n";
					resp_str = "{\"code\":1, \"msg_type\":200}";
					break;
				}

				rapidjson::StringBuffer sb;
				rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
				writer.StartObject();
				writer.Key("msg_type");
				writer.Int(msg_type);
				writer.Key("code");
				writer.Int(1);

				writer.Key("data");
				writer.StartArray();
				for (std::vector<std::map<std::string,std::string> >::const_iterator iter = vmList.cbegin(); iter != vmList.cend(); iter++)
				{
					std::map<std::string,std::string> vmTmp = (*iter);

					writer.StartObject();
					writer.Key("name");
					writer.String(vmTmp["name"].c_str());
					// writer.String(((std::map<std::string,std::string>)(*iter))["name"].c_str());
					writer.Key("uuid");
					writer.String(vmTmp["uuid"].c_str());
					writer.Key("state");
					writer.String(vmTmp["state"].c_str());
					writer.EndObject();
				}
				writer.EndArray();

				writer.EndObject();
				// std::cout << sb.GetString() << std::endl;
				resp_str = (char*)sb.GetString();
				break;
			}
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

