#include <libvirt/libvirt.h>
// #include <libvirt/libvirt-domain.h>
#include <libvirt/virterror.h>

#include <vector>
#include <map>
#include <iostream>


typedef enum
{
	VM_OPCODE_START = 0,
	VM_OPCODE_SHUTDOWN,
	VM_OPCODE_DESTROY,
	VM_OPCODE_UNDEFINE,
	VM_OPCODE_SUSPEND,
	VM_OPCODE_RESUME,
	VM_OPCODE_REBOOT,
	VM_OPCODE_MAX
} VIRSH_VM_OPCODE;

typedef struct _vm_list_item {
	int state;
	char * name;
	char * uuid;
} vm_list_item;

enum name
{
	
};


class Virsh
{
public:
	Virsh();
	Virsh(const char *connName);
	~Virsh();

	int getVmList(std::vector<std::map<std::string,std::string> > &vmList);
	int operateVm(const char * vm_uuid, int op_code);
private:
	// static virConnectPtr conn;
	// static virDomainPtr dom;
	virConnectPtr conn;
};