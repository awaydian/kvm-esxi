
int Virsh::getVmList()
{
	// virDomainPtr ** _allDomains;
	// int * domainNum;
	static virConnectPtr conn = NULL;
	static virDomainPtr dom = NULL;

	// conn = virConnectOpen("qemu:///system");
	conn = virConnectOpen("qemu+ssh://root@192.168.2.236/system?socket=/var/run/libvirt/libvirt-sock");
	// virsh -c qemu+ssh://root@192.168.2.236/system?socket=/var/run/libvirt/libvirt-sock list --all
	// conn = virConnectOpen("qemu+ssh://root@192.168.2.107/system");
	if (conn == NULL) return 1;
	virDomainPtr *allDomains;
	int numDomains = 0;
	int numActiveDomains, numInactiveDomains;
	char **inactiveDomains;
	int *activeDomains;
	int i;
	numActiveDomains = virConnectNumOfDomains(conn);
	numInactiveDomains = virConnectNumOfDefinedDomains(conn);

	int numAllDomain = numActiveDomains + numInactiveDomains;
	allDomains = (virDomainPtr*)malloc(sizeof(virDomainPtr) * numAllDomain);
	// *domainNum = numAllDomain;

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

	char * uuid = (char*)malloc(sizeof(char) * 36);
	char * name = NULL;
	int id = 0;
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
	      if (dom != NULL) virDomainFree(dom);
	      // return 1;
	    }
	    fprintf(stdout, "%d\t%s\t\t%d\t%s\n", id, name, info.state, uuid);
	}


	if(NULL != uuid) free(uuid);
	if(NULL != name) free(name);

	// check status
	if (conn != NULL) virConnectClose(conn);
	if (dom != NULL) virDomainFree(dom);

	// *_allDomains = allDomains;

	return 0;
}
