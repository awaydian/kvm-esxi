#include <map>
#include <iostream>

int main()
{
	std::map<std::string,int> res;
	// res.insert(std::make_pair("a",1));
	res["a"] = 100;

	std::cout << res["a"];

	return 0;
}