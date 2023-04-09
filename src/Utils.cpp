#include "Utils.hpp"

std::string Utils::Trim(const std::string &str)
{
	std::string::size_type first = str.find_first_not_of(' ');

	if (first == std::string::npos)
	{
		return "";
	}

	std::string::size_type last = str.find_last_not_of(' ');

	return str.substr(first, last - first + 1);
}
