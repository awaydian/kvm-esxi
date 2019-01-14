#include <libvirt/libvirt.h>
// #include <libvirt/libvirt-domain.h>
#include <libvirt/virterror.h>

#include <vector>
#include <map>
#include <iostream>


typedef struct _vm_list_item {
	int state;
	char * name;
	char * uuid;
} vm_list_item;


class Virsh
{
public:
	Virsh();
	Virsh(const char *connName);
	~Virsh();

	int getVmList(std::vector<std::map<std::string,std::string> > &vmList);
	int operateVm(char * vm_uuid, int op_code);
private:
	// static virConnectPtr conn;
	// static virDomainPtr dom;
	virConnectPtr conn;
};