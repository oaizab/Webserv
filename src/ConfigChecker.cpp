#include "ConfigChecker.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include <string>
#include <vector>

ConfigChecker::ConfigChecker() : configFilePath("./config/webserv.conf"), fin(configFilePath)
{
	if (not fin.is_open())
	{
		throw FileException("Failed to open config file: " + configFilePath);
	}
	validateConfigFile();
}

ConfigChecker::ConfigChecker(const std::string &configFilePath) : configFilePath(configFilePath), fin(configFilePath)
{
	if (not fin.is_open())
	{
		throw FileException("Failed to open config file" + configFilePath);
	}
	validateConfigFile();
}

ConfigChecker::~ConfigChecker()
{
	fin.close();
}

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
		if (component.find_first_not_of("0123456789") != std::string::npos)
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
	if (port.find_first_not_of("0123456789") != std::string::npos)
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
	const std::string ALLOWED_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-.";

	if (hostname.empty() or hostname.length() > HOSTNAME_MAX_LENGTH)
	{
		return false;
	}
	if (hostname.find_first_not_of(ALLOWED_CHARS) != std::string::npos)
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

	size_t spanLength = sizeStr.find_first_not_of("0123456789");

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
	if (code.find_first_not_of(DIGITS) != std::string::npos)
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

	std::vector<std::string> tokens;

	tokens = Utils::Split(errorPages, ' ');
	if (tokens.size() < 2)
	{
		return false;
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

void ConfigChecker::validateConfigFile()
{
	std::string line;

	while (std::getline(fin, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		line = Utils::Trim(line);
		if (line.empty() or line.front() == '#')
		{
			continue;
		}

		std::vector<std::string> tokens = Utils::Split(line, ' ');

		if (tokens.size() != 2)
		{
			throw ConfigCheckerException("Invalid directive at the top level: " + tokens.front());
		}
		if (tokens.front() != "server")
		{
			throw ConfigCheckerException("Expected 'server' directive at the top level");
		}
		if (tokens.back() != "{")
		{
			throw ConfigCheckerException("Expected opening curly brace '{' after server directive");
		}
		validateServerBlock(fin);
	}
}

void ConfigChecker::validateServerBlock(std::ifstream &fin)
{
	std::string line;
	std::vector<std::string> tokens;

	while (std::getline(fin, line))
	{
		line = Utils::Trim(line);
		std::replace(line.begin(), line.end(), '\t', ' ');
		if (line.empty() or line.front() == '#')
		{
			continue;
		}
		if (line == "}")
		{
			return;
		}
		tokens = Utils::Split(line, ' ');
		tokens.front() == "host"                   ? validateHostDirective(tokens)
		: tokens.front() == "port"                 ? validatePortDirective(tokens)
		: tokens.front() == "server_name"          ? validateServerNameDirective(tokens)
		: tokens.front() == "error_pages"          ? validateErrorPagesDirective(tokens, fin)
		: tokens.front() == "client_max_body_size" ? validateClientMaxBodySizeDirective(tokens)
		: tokens.front() == "location"             ? validateLocationDirective(tokens, fin)
		: throw ConfigCheckerException("Invalid directive: " + tokens.front() + " at server block level");
	}
	throw ConfigCheckerException("Missing end curly brace '}' at server block level");
}

void ConfigChecker::validateHostDirective(const std::vector<std::string> &tokens)
{
	if (tokens.size() != 2)
	{
		throw ConfigCheckerException("host directive requires at least one argument");
	}
	if (not validateHostname(tokens.back()) and not validateIp(tokens.back()))
	{
		throw ConfigCheckerException("Invalid host directive");
	}
}

void ConfigChecker::validatePortDirective(const std::vector<std::string> &tokens)
{
	if (tokens.size() != 2)
	{
		throw ConfigCheckerException("port directive requires at least one argument");
	}
	if (not validatePortNumber(tokens.back()))
	{
		throw ConfigCheckerException("Invalid port directive");
	}
}

void ConfigChecker::validateServerNameDirective(const std::vector<std::string> &tokens)
{
	if (tokens.size() != 2)
	{
		throw ConfigCheckerException("server_name directive requires at least one argument");
	}
	for (size_t i = 1; i < tokens.size(); i++)
	{
		if (not validateHostname(tokens[i]))
		{
			throw ConfigCheckerException("Invalid server name" + tokens[i]);
		}
	}
}

void ConfigChecker::validateErrorPagesDirective(const std::vector<std::string> &tokens, std::ifstream &fin)
{
	std::string line;

	if (tokens.size() != 2)
	{
		throw ConfigCheckerException("Invalid error_pages directive");
	}
	if (tokens.back() != "{")
	{
		throw ConfigCheckerException("Expected opening curly brace '{' after error_pages directive");
	}
	while (std::getline(fin, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		line = Utils::Trim(line);
		if (line.empty() or line.front() == '#')
		{
			continue;
		}
		if (line == "}")
		{
			return;
		}
		if (not validateErrorPages(line))
		{
			throw ConfigCheckerException("Invalid error_pages directive");
		}
	}
	throw ConfigCheckerException("Expected closing curly brace '}' after error_pages directive");
}

void ConfigChecker::validateClientMaxBodySizeDirective(const std::vector<std::string> &tokens)
{
	if (tokens.size() != 2)
	{
		throw ConfigCheckerException("client_max_body_size directive requires only one argument");
	}
	if (not validateSize(tokens.back()))
	{
		throw ConfigCheckerException("client_max_body_size: Invalid size");
	}
}

void ConfigChecker::validateLocationDirective(const std::vector<std::string> &tokens, std::ifstream &fin)
{
	(void) tokens;
	(void) fin;
}
