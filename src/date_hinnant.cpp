#include <iostream>
#include <sstream>

#include "date/date.h"

int main()
{
	//std::istringstream iStr("0000000");
	std::istringstream iStr("0000030");
	std::ostringstream oStr;

	date::sys_days sysDays;

	iStr >> date::parse("%Y%j", sysDays);
	oStr << date::format("%F", sysDays);

	std::cout << oStr.str() << std::endl;

	return 0;
}
