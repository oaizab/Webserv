#include "Server.hpp"
#include <cstring>
#include <iostream>
#include <netdb.h>

Server::Server()
{
	clientMaxBodySize = 10UL*1024*1024;
}

void Server::addListen(const std::string &host, const std::string &port, std::set<std::pair<uint32_t, uint16_t> > &listenParams)
{
	struct addrinfo *result = NULL;
	struct addrinfo hints = {};
	int status = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;	
	status = getaddrinfo(
		host.c_str(),
		port.c_str(),
		&hints,
		&result
	);
	if (status == -1)
	{
		std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
		exit(1);
	}

	std::pair<uint32_t, uint16_t> addr = std::make_pair(
		(reinterpret_cast<struct sockaddr_in *>(result->ai_addr))->sin_addr.s_addr,
		(reinterpret_cast<struct sockaddr_in *>(result->ai_addr))->sin_port
	);
	freeaddrinfo(result);
	if (listenParams.find(addr) != listenParams.end())
	{
		std::cerr << "Duplicate listen address: " << host << ':' << port << std::endl;
		exit(1);
	}
	listenParams.insert(addr);
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
