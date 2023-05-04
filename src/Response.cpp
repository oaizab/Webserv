#include "Response.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "statusCodes.hpp"
#include <fstream>
#include <sstream>
#include <sys/unistd.h>
#include <unistd.h>
#include <vector>

std::string Response::getMessageByStatus(int status)
{
	switch (status)
	{
		case 100:
			return "Continue";
		case 200:
			return "OK";
		case 201:
			return "Created";
		case 301:
			return "Moved Permanently";
		case 302:
			return "Found";
		case 307:
			return "Temporary Redirect";
		case 400:
			return "Bad Request";
		case 403:
			return "Forbidden";
		case 404:
			return "Not Found";
		case 405:
			return "Method Not Allowed";
		case 411:
			return "Length Required";
		case 413:
			return "Payload Too Large";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 505:
			return "HTTP Version Not Supported";
		default:
			throw std::runtime_error("Unknown status code");
	}
}

void Response::error(int status)
{
	_status = status;
	_body = "<html>\n<head>\n\t<title>"
			+ std::to_string(status) + " "
			+ getMessageByStatus(status) + "</title>\n</head>\n<body>\n\t<center><h1>"
			+ std::to_string(status) + " " + getMessageByStatus(status)
			+ "</h1></center>\n\t<hr>\n\t<center>webserv/1.0.0 (Unix) (MacOS/Intel)</center>\n</body>\n</html>\n";
	_contentLength = _body.length();
	_contentType = "text/html";
	_keepAlive = false;
}

Location &Response::matchUri(const std::string &uri, const Server &server)
{
	const std::vector<Location> &locations = server.locations;
	Location *bestMatch = NULL;

	assert(not locations.empty());

	typedef std::vector<Location>::const_iterator locationIterator;

	for (locationIterator it = locations.begin(); it != locations.end(); ++it)
	{
		const std::string &locationUri = it->uri;

		if (Utils::startsWith(uri, locationUri))
		{
			if (bestMatch == NULL or locationUri.length() > bestMatch->uri.length())
			{
				bestMatch = const_cast<Location *>(&(*it));
			}
		}
	}
	return *bestMatch;
}

void Response::error(int status, const Location &location, const Server &server)
{
	std::map<std::string, std::string>::const_iterator iter = server.errorPages.find(std::to_string(status));
	if (iter != server.errorPages.end())
	{
		std::string errorPage = iter->second;
		std::string path = location.root + '/' + errorPage;
		if (access(path.c_str(), F_OK) == 0)
		{
			_status = status;
			std::ifstream ifs(path.c_str());
			std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			_body = str;
			_contentLength = _body.length();
			_contentType = "text/html";
			_keepAlive = false;
		}
		else
			error(status);
	}
	else
		error(status);
}

void Response::generateErrorPage(const Request &req, const Server &server)
{
	const int headerErrors[] = {BAD_REQUEST, LENGTH_REQUIRED, PAYLOAD_TOO_LARGE, NOT_IMPLEMENTED, HTTP_VERSION_NOT_SUPPORTED};
	if (std::find(std::begin(headerErrors), std::end(headerErrors), req.status()) != std::end(headerErrors))
	{
		error(req.status());
		return;
	}
	Location &location = matchUri(req.uri(), server);
	error(req.status(), location, server);
}

void Response::generateResponse(const Request &req, const Server &server)
{
	if (req.status() != OK)
	{
		generateErrorPage(req, server);
		return;
	}
	error(OK);  // HACK: this is a hack to avoid having to check if the status is OK in the following code
}

std::string Response::toString() const
{
	std::stringstream stream;

	stream << "HTTP/1.1 " << _status << " " << getMessageByStatus(_status) << "\r\n";
	stream << "Server: webserv/1.0.0 (Unix) (MacOS/Intel)\r\n";
	stream << "Content-Type: " << _contentType << "\r\n";
	stream << "Content-Length: " << _contentLength << "\r\n";
	stream << "Connection: " << (_keepAlive ? "keep-alive" : "close") << "\r\n";
	stream << "\r\n";
	stream << _body;
	return stream.str();
}
