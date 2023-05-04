#pragma once

#include "Location.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <string>
#include <sys/_types/_size_t.h>

class Response
{
	public:
		void generateResponse(const Request &req, const Server &server);
		std::string toString() const;

	private:
		int _status;
		std::string _body;
		bool _keepAlive;
		std::string _contentType;
		size_t _contentLength;

		void error(int status);
		void error(int status, const Location &location, const Server &server);
		static std::string getMessageByStatus(int status);
		Location &matchUri(const std::string &uri, const Server &server);
		void generateErrorPage(const Request &req, const Server &server);
};
