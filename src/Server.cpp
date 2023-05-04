#include "Server.hpp"

Server::Server()
{
	clientMaxBodySize = 10UL*1024*1024;
}

void Server::addListen(const std::string &host, const std::string &port)
{
	listen.push_back(std::make_pair(host, port));
}

void Server::addServerName(const std::string &serverName)
{
	serverNames.push_back(serverName);
}

void Server::addErrorPage(const std::string &errorCode, const std::string &errorPage)
{
	if (errorPages.find(errorCode) == errorPages.end())
		errorPages[errorCode] = errorPage;
}

void Server::addLocation(const Location &location)
{
	locations.push_back(location);
}
