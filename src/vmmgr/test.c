#include "MyRmq.h"
#include "virsh/Virsh.h"

#include <iostream>
#include <vector>
#include <map>


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"



int main()
{	
	Virsh vir("qemu+ssh://root@192.168.88.223/system?socket=/var/run/libvirt/libvirt-sock");

	vir.operateVm("544f4d91-f266-445b-8804-69a9221a339a",6);
	return 0;

	int msg_type = 200;
	char * resp_str = NULL;

	std::vector<std::map<std::string,std::string> > vmList;
	if( vir.getVmList(vmList)) {
		std::cout << "getVmList error\n";
		return 1;
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
	// const char * json = "{\"msg_type\": 200}";
	//// {"msg_type": 200}
	std::cout << "list: \n" << resp_str << std::endl;




	//

	// rapidjson::Document doc;
	// doc.Parse(json);
	// assert(doc.HasMember("msg_type"));
	// assert(doc["msg_type"].IsInt());
	// std::cout << doc["msg_type"].GetInt() <<std::endl;

	return 0;
}

