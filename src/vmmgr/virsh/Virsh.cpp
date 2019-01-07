#include "Virsh.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <string>
#include <sstream>

std::string int2str(int i)
{
	std::stringstream s;
	s << i;
	return s.str();
}


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
 
	    std::map<std::string,std::string> tmp;
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

