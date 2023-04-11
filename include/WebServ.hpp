#pragma once

#include "Location.hpp"
#include "Server.hpp"
#include <string>
#include <sys/_types/_size_t.h>
#include <utility>
#include <vector>

class WebServ
{
	public:
	// Methods
	static void startServers(const std::string &configFilePath);
	static void startServers();
	static void parseServers(const std::string &configFilePath);
	static Server parseServerBlock(std::ifstream &fin);
	static std::pair<std::string, std::string> parseListenParams(const std::string &param);
	static void parseErrorPagesBlock(std::ifstream &fin, Server &server);
	static size_t parseClientMaxBodySizeParam(const std::string &param);
	static Location parseLocationBlock(std::ifstream &fin, const std::string &uri);

	// Attributes
	static std::vector<Server> servers;
};
