#include "Server.hpp"

Server::Server()
{
	host = "";
	port = "8080";
	clientMaxBodySize = 0;
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
