#pragma once

#include "Location.hpp"

#include <map>
#include <string>
#include <sys/_types/_size_t.h>
#include <utility>
#include <vector>

class Server
{
	public:
	// Constructors
	Server();

	// Methods
	void addListen(const std::string &host, const std::string &port);
	void addServerName(const std::string &serverName);
	void addErrorPage(const std::string &errorCode, const std::string &errorPage);
	void addLocation(const Location &location);

	// Attributes
	std::vector<std::pair<std::string, std::string> > listen;
	std::vector<std::string> serverNames;
	std::map<std::string, std::string> errorPages;
	size_t clientMaxBodySize;
	std::vector<Location> locations;
};
