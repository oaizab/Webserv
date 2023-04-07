#pragma once

#include <cstring>
#include <set>
#include <sstream>
#include <string>
#include <vector>

class ConfigChecker
{
	public:
		// Static methods
		static bool validateIp(const std::string &ipAddress);
		static bool validatePortNumber(const std::string &port);
		static bool validateHostname(const std::string &hostname);
		static bool validateSize(const std::string &sizeStr);
		static bool validateHttpMethod(const std::string &method);
};
