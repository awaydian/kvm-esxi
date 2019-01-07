
#include <iostream>
#include <string>
#include <sstream>

std::string int2str(int i)
{
	std::stringstream s;
	s << i;
	return s.str();
}

int main()
{
	int i = 111;
	std::string ss = int2str(i);
	std::cout << ss << std::endl;

	return 0;
}