#pragma once

#include "Location.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include <string>
#include "WebServ.hpp"

class Response
{
	public:
		Response();

		void generateResponse(Request &req, const Server &server, const client_info &client);
		std::string toString() const; 

	private:
		int _status;
		std::string _body;
		std::string _contentType;
		size_t _contentLength;
		std::string _location;
		std::vector<std::string> _cookies;

		void error(int status);
		void error(int status, const Location &location, const Server &server);
		static std::string getMessageByStatus(int status);
		Location *matchUri(const std::string &uri, const Server &server);
		void generateErrorPage(const Request &req, const Server &server);

		struct Entry
		{
			std::string name;
			bool isDirectory;
		};
		std::string generateDirectoryListing(const std::string &path, const std::string &uri);
		static bool compareEntries(const Entry &entA, const Entry &entB);

		std::string getFileContent(const std::string &path, Request &req, const Server &server);

		void GET(Request &req, const Server &server, const client_info &client);
		void DELETE(Request &req, const Server &server);
		void PUT(Request &req, const Server &server);
};
