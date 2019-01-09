#include "Virsh.h"

#include <map>
#include <vector>
#include <iostream>

int main()
{
	std::vector<std::map<std::string, std::string> > vmList;
	// std::map<std::string,std::string> vmList;

	Virsh vir("qemu+ssh://root@192.168.2.236/system?socket=/var/run/libvirt/libvirt-sock");
	if( vir.getVmList(vmList)) {
		std::cout << "getVmList error\n";
	}

	std::cout << "vm count: " << vmList.size() << std::endl;
	std::cout << "vm id: " << vmList[0]["id"] << std::endl;
	std::cout << "vm name: " << vmList[0]["name"] << std::endl;
	std::cout << "vm uuid: " << vmList[0]["uuid"] << std::endl;
	std::cout << "vm state: " << vmList[0]["state"] << std::endl;


}