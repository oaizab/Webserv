#include "ConfigChecker.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include <string>
#include <vector>

ConfigChecker::ConfigChecker() :
	configFilePath("./config/webserv.conf"),
	fin(configFilePath),
	clientMaxBodySizeDirectiveCount(0),
	errorPagesDirectiveCount(0),
	serverDirectiveCount(0)
{
	if (not fin.is_open())
	{
		throw FileException("Failed to open config file: " + configFilePath);
	}
	validateConfigFile();
}

ConfigChecker::ConfigChecker(const std::string &configFilePath) :
	configFilePath(configFilePath),
	fin(configFilePath),
	clientMaxBodySizeDirectiveCount(0),
	errorPagesDirectiveCount(0),
	serverDirectiveCount(0)
{
	if (not fin.is_open())
	{
		throw FileException("Failed to open config file: " + configFilePath);
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

/**
 * @brief Validate size in bytes, kilobytes, megabytes or gigabytes
 *
 */
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

/**
 * @brief Validate HTTP error codes (from 300 to 599)
 *
 */
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

/**
 * @brief Validate error pages line in the format "error_code1 error_code2 ... error_codeN path_to_error_page"
 *
 */
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

		if (tokens.front() != "server")
		{
			throw ConfigCheckerException("Invalid directive at the top level: " + tokens.front());
		}
		if (tokens.size() != 2 or tokens.back() != "{")
		{
			throw ConfigCheckerException("Expected opening curly brace '{' after server directive");
		}
		validateServerBlock(fin);
		++serverDirectiveCount;
	}
	if (serverDirectiveCount == 0)
	{
		throw ConfigCheckerException("No server blocks found");
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
		throw ConfigCheckerException("host directive requires one argument, host name or IP address");
	}
	if (not validateHostname(tokens.back()) and not validateIp(tokens.back()))
	{
		throw ConfigCheckerException("Invalid host directive: " + tokens.back());
	}
}

void ConfigChecker::validatePortDirective(const std::vector<std::string> &tokens)
{
	if (tokens.size() != 2)
	{
		throw ConfigCheckerException("port directive requires one argument, port number");
	}
	if (not validatePortNumber(tokens.back()))
	{
		throw ConfigCheckerException("Invalid port number: " + tokens.back());
	}
}

void ConfigChecker::validateServerNameDirective(const std::vector<std::string> &tokens)
{
	if (tokens.size() < 2)
	{
		throw ConfigCheckerException("server_name directive requires at least one argument");
	}
	for (size_t i = 1; i < tokens.size(); i++)
	{
		if (not validateHostname(tokens[i]))
		{
			throw ConfigCheckerException("Invalid server name: " + tokens[i]);
		}
	}
}

void ConfigChecker::validateErrorPagesDirective(const std::vector<std::string> &tokens, std::ifstream &fin)
{
	std::string line;

	if (tokens.size() != 2 or tokens.back() != "{")
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
			++errorPagesDirectiveCount;
			if (errorPagesDirectiveCount > 1)
			{
				throw ConfigCheckerException("error_pages directive can only be used once");
			}
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
		throw ConfigCheckerException("client_max_body_size: Invalid size: " + tokens.back());
	}
	++clientMaxBodySizeDirectiveCount;
	if (clientMaxBodySizeDirectiveCount > 1)
	{
		throw ConfigCheckerException("client_max_body_size directive can only be used once");
	}
}

void ConfigChecker::validateLocationDirective(const std::vector<std::string> &tokens, std::ifstream &fin)
{
	(void) tokens;
	(void) fin;
}
