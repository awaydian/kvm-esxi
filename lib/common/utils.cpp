#include "utils.h"

#include <sstream>
#include <string>

#include <stdio.h>




std::string int2str(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

int execCmd(const char *cmd)
{
	FILE *fp = NULL;
	fp = popen(cmd, "r");
	if (!fp) {
		std::cout << "execCmd failed. cmd: "  << cmd << std::endl;
		return 1;
	}
	std::cout << "execCmd: " << cmd << std::endl;
	char buf[100];
	while (memset(buf,0,sizeof(buf)),fgets(buf,sizeof(buf)-1,fp) != 0)
	{
		std::cout << buf << std::endl;
	}
	pclose(fp);
	return 0;
}