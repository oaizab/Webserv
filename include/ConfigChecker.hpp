#pragma once

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

class ConfigChecker
{
	public:
		// Static methods
		static bool validateIp(const std::string &ipAddress);
};
