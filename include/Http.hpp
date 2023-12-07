#pragma once

#include <string>

#include "Request.hpp"
#include "Response.hpp"
#include "WebServ.hpp"

class Http
{
	public:
		Http();
		void readRequest(int socketfd, const client_info &client);
		bool sendResponse(int socketfd);
		static Server &matchHost(const std::string &host, int socketFd);

	private:
		Request _req;
		Response _res;
		bool _isResponseGenerated;
		size_t _bytesSent;
		std::string _response;

};
