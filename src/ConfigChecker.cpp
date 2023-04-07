#include "ConfigChecker.hpp"
#include <cstdio>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>

bool ConfigChecker::validateIp(const std::string &ipAddress)
{
	std::vector<int> components;
	std::stringstream stream(ipAddress);
	std::string component;

	while (std::getline(stream, component, '.'))
	{
		if (component.empty() or component.size() > 3)
		{
			return false;
		}
		if (std::strspn(component.c_str(), "0123456789") != component.size())
		{
			return false;
		}

		int value = std::stoi(component);

		if (value < 0 or value > 255)
		{
			return false;
		}
		components.push_back(value);
	}
	return components.size() == 4;
}

bool ConfigChecker::validatePortNumber(const std::string &port)
{
	const uint16_t MAX_PORT = 65535;

	if (port.empty() or port.size() > 5)
	{
		return false;
	}
	if (std::strspn(port.c_str(), "0123456789") != port.size())
	{
		return false;
	}

	int portNumber = std::stoi(port);

	return portNumber > 0 and portNumber <= MAX_PORT;
}

bool ConfigChecker::validateHostname(const std::string &hostname)
{
	const int HOSTNAME_MAX_LENGTH = 255;
	const int SEGMENT_MAX_LENGTH = 63;

	if (hostname.empty() or hostname.length() > HOSTNAME_MAX_LENGTH)
	{
		return false;
	}
	if (std::strspn(hostname.c_str(), "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.") != hostname.size())
	{
		return false;
	}
	if (hostname.front() == '-' or hostname.front() == '.' or hostname.back() == '-' or hostname.back() == '.')
	{
		return false;
	}

	std::stringstream stream(hostname);
	std::string segment;

	while (std::getline(stream, segment, '.'))
	{
		if (segment.length() > SEGMENT_MAX_LENGTH)
		{
			return false;
		}
	}
	return true;
}

bool ConfigChecker::validateSize(const std::string &sizeStr)
{
	const std::string SIZE_SUFFIXES = "kmgKMG";
	const std::set<char> SIZE_SUFFIXES_SET(SIZE_SUFFIXES.begin(), SIZE_SUFFIXES.end());

	if (sizeStr.empty())
	{
		return false;
	}

	size_t spanLength = std::strspn(sizeStr.c_str(), "0123456789");

	if (spanLength < sizeStr.size() - 1)
	{
		return false;
	}
	if (spanLength == sizeStr.size() - 1 and SIZE_SUFFIXES_SET.find(sizeStr.back()) == SIZE_SUFFIXES_SET.end())
	{
		return false;
	}
	return true;
}

bool ConfigChecker::validateHttpMethod(const std::string &method)
{
	const std::string ALLOWED_METHODS[] = {"GET", "POST", "DELETE"};

	return std::find(std::begin(ALLOWED_METHODS), std::end(ALLOWED_METHODS), method) != std::end(ALLOWED_METHODS);
}

bool ConfigChecker::validateErrorCode(const std::string &code)
{
	const char *DIGITS = "0123456789";

	if (code.empty() or code.size() > 3)
	{
		return false;
	}
	if (std::strspn(code.c_str(), DIGITS) != code.size())
	{
		return false;
	}
	int errorCode = std::stoi(code);
	return errorCode >= 300 and errorCode <= 599;
}

bool ConfigChecker::validateErrorPages(const std::string &errorPagesParam)
{
	std::string errorPages = errorPagesParam;
	if (errorPages.empty())
	{
		return false;
	}
	std::replace(errorPages.begin(), errorPages.end(), '\t', ' ');

	std::istringstream inputString(errorPages);
	std::string token;
	std::vector<std::string> tokens;

	while (std::getline(inputString, token, ' '))
	{
		if (token.empty())
		{
			continue;
		}
		tokens.push_back(token);
	}
	for (size_t i = 0; i < tokens.size() - 1; i++)
	{
		if (not validateErrorCode(tokens[i]))
		{
			return false;
		}
	}
	return true;
}
