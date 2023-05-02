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

std::vector<std::string> Utils::Split(const std::string &str, char delim)
{
	std::vector<std::string> tokens;
	std::string::size_type start = 0;
	std::string::size_type end = str.find_first_of(delim);

	while (end != std::string::npos)
	{
		if (end != start)
		{
			tokens.push_back(str.substr(start, end - start));
		}
		start = end + 1;
		end = str.find_first_of(delim, start);
	}

	std::string last = str.substr(start);

	if (not last.empty())
	{
		tokens.push_back(last);
	}
	return tokens;
}

std::vector<std::string> Utils::reqSplit(const std::string &str)
{
	std::vector<std::string> tokens;
	std::string::size_type start = 0;
	std::string::size_type end = str.find_first_of('\n');

	while (end != std::string::npos)
	{
		if (end >= start)
		{
			tokens.push_back(str.substr(start, end - start + 1));
		}
		start = end + 1;
		end = str.find_first_of('\n', start);
	}

	std::string last = str.substr(start);

	if (not last.empty())
	{
		tokens.push_back(last);
	}
	return tokens;
}

bool Utils::endsWith(const std::string &str, const std::string &suffix)
{
	if (str.length() < suffix.length())
	{
		return false;
	}

	const std::string end = str.substr(str.length() - suffix.length());

	return end == suffix;
}
