#include "Http.hpp"
#include "Server.hpp"
#include "WebServ.hpp"
#include "statusCodes.hpp"
#include <cstdio>
#include <string>
#include <sys/_types/_size_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <vector>

Http::Http()
{
	_isResponseGenerated = false;
	_bytesSent = 0;
}

void Http::readRequest(int socketfd)
{
	std::string request;
	char buf[BUFSIZ + 1];
	ssize_t bytes_read = 0;

	bytes_read = recv(socketfd, buf, BUFSIZ, 0);
	if (bytes_read == -1)
	{
		_req.setStatus(INTERNAL_SERVER_ERROR);
		Server &server = matchHost(_req.host(), socketfd);
		_res.generateResponse(_req, server);
		_response = _res.toString();
		_isResponseGenerated = true;
		return;
	}
	if (bytes_read == 0)
	{
		_req.setStatus(BAD_REQUEST);
		Server &server = matchHost(_req.host(), socketfd);
		_res.generateResponse(_req, server);
		_response = _res.toString();
		_isResponseGenerated = true;
		return;
	}
	Server &server = matchHost(_req.host(), socketfd);
	buf[bytes_read] = '\0';
	request = buf;
	if (not _req.readRequest(request, server.clientMaxBodySize))
	{
		Server &server = matchHost(_req.host(), socketfd);
		_res.generateResponse(_req, server);
		_response = _res.toString();
		_isResponseGenerated = true;
	}
}

bool Http::sendResponse(int socketfd)
{
	if (_isResponseGenerated)
	{
		size_t bytesToSend = _response.size() - _bytesSent;
		if (bytesToSend > BUFSIZ)
			bytesToSend = BUFSIZ;
		ssize_t bytesSentNow = send(socketfd, _response.c_str() + _bytesSent, bytesToSend, 0);
		if (bytesSentNow == -1)
			return false;
		_bytesSent += bytesSentNow;
		return not (_bytesSent == _response.size());
		// TODO(oaizab): check if we need to close the connection
	}
	return true;
}

Server &Http::matchHost(const std::string &host, int socketFd)
{
	std::vector<Server *> &servers = WebServ::serversBySocket[socketFd];
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::vector<std::string> &hosts = (*it)->serverNames;
		std::vector<std::string>::iterator it2 = std::find(hosts.begin(), hosts.end(), host);
		if (it2 != hosts.end())
			return **it;
	}
	return *(servers[0]);
}
