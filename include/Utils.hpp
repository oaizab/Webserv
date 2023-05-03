#pragma once

#include <string>
#include <vector>

namespace Utils
{
	std::string Trim(const std::string &str);
	std::vector<std::string> Split(const std::string &str, char delim);
	std::vector<std::string> reqSplit(const std::string &str);
	bool endsWith(const std::string &str, const std::string &suffix);
	bool startsWith(const std::string &str, const std::string &prefix);
};
