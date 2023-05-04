#pragma once

#include <string>
#include <sys/_types/_ssize_t.h>

#include "Request.hpp"
#include "Response.hpp"

class Http
{
	public:
		Http();
		void readRequest(int socketfd);
		bool sendResponse(int socketfd);

	private:
		Request _req;
		Response _res;
		bool _isResponseGenerated;
		size_t _bytesSent;
		std::string _response;

		Server &matchHost(const std::string &host, int socketFd);
};
