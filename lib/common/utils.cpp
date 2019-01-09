#include "utils.h"

#include <sstream>
#include <string>


std::string int2str(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}