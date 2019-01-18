#include "Virsh.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "common/utils.h"

// virConnectPtr Virsh::conn = NULL;
Virsh::Virsh()
{
	// "qemu+ssh://root@192.168.2.236/system?socket=/var/run/libvirt/libvirt-sock"
	conn = virConnectOpen("qemu:///system");
	assert(conn);
}

Virsh::Virsh(const char * connURL)
{
	conn = virConnectOpen(connURL);
	assert(conn);
}

Virsh::~Virsh()
{
	if (conn != NULL) {
		virConnectClose(conn);
		conn = NULL;
	}
}

int Virsh::getVmList(std::vector<std::map<std::string,std::string> > &vmList)
{
	virDomainPtr *allDomains;
	int numDomains = 0;
	int numActiveDomains, numInactiveDomains;
	int numAllDomain;
	char **inactiveDomains;
	int *activeDomains;
	int i;
	char *uuid,*name;
	int id;

	numActiveDomains = virConnectNumOfDomains(conn);
	numInactiveDomains = virConnectNumOfDefinedDomains(conn);

	numAllDomain = numActiveDomains + numInactiveDomains;

	allDomains = (virDomainPtr*)malloc(sizeof(virDomainPtr) * numAllDomain);
	inactiveDomains = (char**)malloc(sizeof(char *) * numInactiveDomains);
	activeDomains = (int*)malloc(sizeof(int) * numActiveDomains);

	numActiveDomains = virConnectListDomains(conn,
	    activeDomains,
	    numActiveDomains);
	numInactiveDomains = virConnectListDefinedDomains(conn,
	    inactiveDomains,
	    numInactiveDomains);

	for (i = 0 ; i < numActiveDomains ; i++) {
	    allDomains[numDomains] = virDomainLookupByID(conn, activeDomains[i]);
	    numDomains++;
	}
	for (i = 0 ; i < numInactiveDomains ; i++) {
	    allDomains[numDomains] = virDomainLookupByName(conn, inactiveDomains[i]);
	    free(inactiveDomains[i]);
	        numDomains++;
	}
	if(NULL != activeDomains) free(activeDomains);
	if(NULL != inactiveDomains) free(inactiveDomains);

	uuid = (char*)malloc(sizeof(char) * 36);
	name = NULL;
	id = 0;
	virDomainInfo info;
	virErrorPtr error = NULL;
	std::map<std::string,std::string> tmp;

	for (i = 0; i < numAllDomain; i++) {
	    id = virDomainGetID(allDomains[i]);
	    name = (char*)virDomainGetName(allDomains[i]);
	    virDomainGetUUIDString(allDomains[i], uuid);

	    if (virDomainGetInfo(allDomains[i], &info) < 0) {
	      error = virGetLastError();
	      fprintf(stderr, "virDomainGetInfo failed: %s\n", error->message);
	      virFreeError(error);
	      if (conn != NULL) virConnectClose(conn);
	      return 1;
	    }
	    // fprintf(stdout, "%d\t%s\t\t%d\t%s\n", id, name, info.state, uuid);
 
	    tmp["id"] = int2str(id);
	    tmp["name"] = name;
	    tmp["uuid"] = uuid;
	    tmp["state"] = int2str(info.state);

	    vmList.insert(vmList.begin(),tmp);

	}

	if(NULL != uuid) free(uuid);
	if(NULL != name) free(name);

	return 0;
}

int Virsh::operateVm(const char *vm_uuid, int op_code)
{
	virDomainPtr dom = NULL;
	int flag = 1;
	std::cout << "operateVm uuid: " << vm_uuid << " op: " << op_code << std::endl;
	dom = virDomainLookupByUUIDString(conn, vm_uuid);
	if (dom == NULL)
	{
		std::cout << "domain lookup failed\n";
		return 1;
	}

	switch (op_code){
		case VM_OPCODE_START:
		{
			std::string cmd = "virsh start ";
			cmd += vm_uuid;
			flag = execCmd(cmd.c_str());
			if (flag != 0)	std::cout << "domain start failed\n";
			break;
		}
		case VM_OPCODE_SHUTDOWN:
		{
			flag = virDomainShutdown(dom);
			if (flag != 0)	std::cout << "domain shutdown failed\n";
			break;
		}
		case VM_OPCODE_DESTROY:
		{
			flag = virDomainDestroy(dom);
			if (flag != 0)	std::cout << "domain destroy failed\n";
			break;
		}
		case VM_OPCODE_UNDEFINE:
		{
			flag = virDomainUndefine(dom);
			if (flag != 0)	std::cout << "domain undefine failed\n";
			break;
		}
		case VM_OPCODE_SUSPEND:
		{
			flag = virDomainSuspend(dom);
			if (flag != 0)	std::cout << "domain suspend failed\n";
			break;
		}
		case VM_OPCODE_RESUME:
		{
			flag = virDomainResume(dom);
			if (flag != 0)	std::cout << "domain resume failed\n";
			break;
		}
		case VM_OPCODE_REBOOT:
		{
			flag = virDomainReboot(dom,VIR_DOMAIN_REBOOT_DEFAULT);
			if (flag != 0)	std::cout << "domain reboot failed\n";
			break;
		}

		default:
			break;
	}
	return flag;
}




