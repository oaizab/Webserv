#include "Response.hpp"
#include "Utils.hpp"

std::string getMessageByStatus(int status)
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
			+ "</h1></center>\n<hr>\n\n\t<center>webserv/1.0.0 (Unix) (MacOS/Intel)</center></body></html>";
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
