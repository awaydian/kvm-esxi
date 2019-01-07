#include "utils.h"



std::string int2str(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.c_str();
}