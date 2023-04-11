#include "WebServ.hpp"
#include "ConfigChecker.hpp"
#include "Exception.hpp"
#include "Location.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <fstream>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>

std::vector<Server> WebServ::servers;

void WebServ::startServers()
{}

void WebServ::startServers(const std::string &configFilePath)
{
	(void)configFilePath;
}

void WebServ::parseServers(const std::string &configFilePath)
{
	std::ifstream fin(configFilePath);
	if (not fin.is_open())
		throw FileException("Failed to open config file: " + configFilePath);
	
	std::string line;
	while (std::getline(fin, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		line = Utils::Trim(line);
		if (line.empty() or line[0] == '#')
			continue;
		
		servers.push_back(parseServerBlock(fin));
	}
}

Server WebServ::parseServerBlock(std::ifstream &fin)
{
	Server server;
	std::string line;

	while (std::getline(fin, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		line = Utils::Trim(line);
		if (line.empty() or line[0] == '#')
			continue;
		
		if (line == "}")
			break;
		
		std::vector<std::string> tokens = Utils::Split(line, ' ');
		if (tokens.front() == "listen")
		{
			for (size_t i = 1; i < tokens.size(); ++i)
			{	
				std::pair<std::string, std::string> listenParams = parseListenParams(tokens[i]);
				server.addListen(listenParams.first, listenParams.second);
			}
		}
		else if (tokens.front() == "server_name")
		{
			for (size_t i = 1; i < tokens.size(); ++i)
				server.addServerName(tokens[i]);
		}
		else if (tokens.front() == "error_pages")
		{
			parseErrorPagesBlock( fin, server);
		}
		else if (tokens.front() == "client_max_body_size")
		{
			server.clientMaxBodySize = parseClientMaxBodySizeParam(tokens[1]);
		}
		else if (tokens.front() == "location")
		{
			server.addLocation(parseLocationBlock(fin, tokens[1]));
		}
	}
	return server;
}

std::pair<std::string, std::string> WebServ::parseListenParams(const std::string &param)
{
	std::pair<std::string, std::string> listenParams;
	std::vector<std::string> tokens = Utils::Split(param, ':');
	if (tokens.size() == 1 and not ConfigChecker::validatePortNumber(tokens[0]))
	{
		listenParams.first = tokens[0];
		listenParams.second = "8080";
	}
	else if (tokens.size() == 2)
	{
		listenParams.first = tokens[0];
		listenParams.second = tokens[1];
	}
	else
	{
		listenParams.first = "0.0.0.0";
		listenParams.second = tokens[0];
	}
	return listenParams;
}

void WebServ::parseErrorPagesBlock(std::ifstream &fin, Server &server)
{
	std::string line;
	while (std::getline(fin, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		line = Utils::Trim(line);
		if (line.empty() or line[0] == '#')
			continue;
		
		if (line == "}")
			break;
		
		std::vector<std::string> tokens = Utils::Split(line, ' ');
		for (size_t i = 0; i < tokens.size() - 1; ++i)
			server.addErrorPage(tokens[i], tokens.back());
	}
}

size_t WebServ::parseClientMaxBodySizeParam(const std::string &param)
{
	const std::string SIZE_SUFFIXES = "kKmMgG";
	size_t size = 0;
	size_t digitsLength = param.find_first_not_of("0123456789");
	if (digitsLength == std::string::npos)
		size = std::stoul(param);
	else
	{
		size = std::stoul(param.substr(0, digitsLength));
		char suffix = param[digitsLength];
		if (suffix == 'k' or suffix == 'K')
			size = size * 1024;
		else if (suffix == 'm' or suffix == 'M')
			size = size * 1024 * 1024;
		else if (suffix == 'g' or suffix == 'G')
			size = size * 1024 * 1024 * 1024;
	}
	return size;
}

Location WebServ::parseLocationBlock(std::ifstream &fin, const std::string &uri)
{
	Location location;
	std::string line;

	location.uri = uri;
	while (std::getline(fin, line))
	{
		std::replace(line.begin(), line.end(), '\t', ' ');
		line = Utils::Trim(line);
		if (line.empty() or line[0] == '#')
			continue;
		
		if (line == "}")
			break;
		
		std::vector<std::string> tokens = Utils::Split(line, ' ');
		if (tokens.front() == "root")
		{
			location.root = tokens[1];
		}
		else if (tokens.front() == "index")
		{
			for (size_t i = 1; i < tokens.size(); ++i)
				location.addIndex(tokens[i]);
		}
		else if (tokens.front() == "autoindex")
		{
			location.autoIndex = tokens[1] == "on";
		}
		else if (tokens.front() == "upload")
		{
			if (tokens[1] == "off")
				location.upload = false;
			else
			{
				location.upload = true;
				location.uploadPath = tokens[1];
			}
		}
		else if (tokens.front() == "redirect")
		{
			location.createRedirection(tokens[2], tokens[1]);
		}
		else if (tokens.front() == "allowed_methods")
		{
			for (size_t i = 1; i < tokens.size(); ++i)
				location.addAllowedMethod(tokens[i]);
		}
		// TODO(): parse cgi
	}
	return location;
}
