#include "Response.hpp"
#include "MimeTypes.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "statusCodes.hpp"
#include <dirent.h>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <vector>
#include "WebServ.hpp"
#include <Cgi.hpp>


Response::Response()
{
	_status = 0;
	_contentLength = 0;
}

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
		case 204:
			return "No Content";
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
		case 409:
			return "Conflict";
		case 411:
			return "Length Required";
		case 413:
			return "Payload Too Large";
		case 500:
			return "Internal Server Error";
		case 501:
			return "Not Implemented";
		case 502:
			return "Bad Gateway";
		case 504:
			return "Gateway Timeout";
		case 505:
			return "HTTP Version Not Supported";
		default:
			throw std::runtime_error("Unknown status code" + std::to_string(status));
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
}

Location *Response::matchUri(const std::string &uri, const Server &server)
{
	const std::vector<Location> &locations = server.locations;
	Location *bestMatch = NULL;
	Location *rootLocation = NULL;

	assert(not uri.empty());
	assert(not locations.empty());

	typedef std::vector<Location>::const_iterator locationIterator;

	for (locationIterator it = locations.begin(); it != locations.end(); ++it)
	{
		const std::string &locationUri = it->uri;

		if (locationUri == "/")
		{
			rootLocation = const_cast<Location *>(&(*it));
			continue;
		}

		if (Utils::startsWith(uri, locationUri))
		{
			size_t nextCharIndex = locationUri.length();

			if (uri.length() > nextCharIndex and uri[nextCharIndex] != '/')
			{
				continue;
			}
			if (bestMatch == NULL or locationUri.length() > bestMatch->uri.length())
			{
				bestMatch = const_cast<Location *>(&(*it));
			}
		}
	}

	if (bestMatch == NULL)
	{
		if (rootLocation != NULL)
			return rootLocation;
		return NULL;
	}
	return bestMatch;
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
	Location *location = matchUri(req.uri(), server);
	if (location == NULL)
		error(req.status());
	else
		error(req.status(), *location, server);
}

void Response::generateResponse(Request &req, const Server &server, const client_info &client)

{
	if (req.status() != OK)
	{
		generateErrorPage(req, server);
		return;
	}

	const Location *location = matchUri(req.uri(), server);

	if (location == NULL)
	{
		error(NOT_FOUND);
		return;
	}
	if (location->allowedMethods.find(req.method()) == location->allowedMethods.end())
	{
		error(METHOD_NOT_ALLOWED);
		return;
	}

	if (location->redirect)
	{
		_status = std::stoi(location->redirectCode);
		_location = location->redirectUrl;
		return;
	}

	if (req.method() == "GET")
	{
		GET(req, server, client);
	}
	else if (req.method() == "DELETE")
	{
		DELETE(req, server);
	}
	else if (req.method() == "POST")
	{
		const std::string extension = Utils::getExtension(req.uri());

		if (location->cgi.find(extension) != location->cgi.end())
		{
			// TODO: CGI coming soon...
			_status = OK;
			_body = "<h1>CGI coming soon...</h1>";
			_contentLength = _body.length();
			_contentType = "text/html";
		}
		else
			GET(req, server, client);
	}
	else if (req.method() == "PUT")
	{
		PUT(req, server);
	}
	else
	{
		error(NOT_IMPLEMENTED);
	}
}

std::string Response::toString() const
{
	std::stringstream stream;

	stream << "HTTP/1.1 " << _status << " " << getMessageByStatus(_status) << "\r\n";
	stream << "Server: webserv/1.0.0 (Unix) (MacOS/Intel)\r\n";
	if (_status != NO_CONTENT and _status != CREATED)
	{
		stream << "Content-Type: " << _contentType << "\r\n";
		stream << "Content-Length: " << _contentLength << "\r\n";
	}
	stream << "Connection: close\r\n";
	if (not _location.empty())
	{
		stream << "Location: " << _location << "\r\n";
	}
	stream << "\r\n";
	if (_status != NO_CONTENT and _status != CREATED)
		stream << _body;
	return stream.str();
}

std::string Response::generateDirectoryListing(const std::string &path, const std::string &_uri)
{
	std::stringstream stream;
	std::string uri = _uri;
	stream << "<html>\n<head>\n\t<title>Index of " << uri << "</title>\n</head>\n<body>\n\t<h1>Index of " << uri << "</h1>\n\t<hr>\n\t<ul>\n";
	if (uri.back() == '/')
		uri.pop_back();
	std::vector<Entry> files;
	DIR *dir = NULL;
	struct dirent *ent = NULL;
	dir = opendir(path.c_str());
	if (dir != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_name[0] != '.')
			{
				Entry entry;
				entry.name = ent->d_name;
				entry.isDirectory = ent->d_type == DT_DIR;
				files.push_back(entry);
			}
		}
		closedir(dir);
	}
	else
	{
		error(INTERNAL_SERVER_ERROR);
		return _body;
	}
	std::sort(files.begin(), files.end(), compareEntries);
	for (std::vector<Entry>::iterator it = files.begin(); it != files.end(); ++it)
	{
		if (it->isDirectory)
			stream << "\t\t<li><a href=\"" << uri << '/' << it->name << "/\">" << it->name << "/</a></li>\n";
		else
			stream << "\t\t<li><a href=\"" << uri << '/' << it->name << "\">" << it->name << "</a></li>\n";
	}
	stream << "\t</ul>\n\t<hr>\n\t<center>webserv/1.0.0 (Unix) (MacOS/Intel)</center>\n</body>\n</html>\n";
	return stream.str();
}

bool Response::compareEntries(const Entry &entA, const Entry &entB)
{
	if (entA.isDirectory and not entB.isDirectory)
		return true;
	if (not entA.isDirectory and entB.isDirectory)
		return false;
	return entA.name < entB.name;
}

std::string Response::getFileContent(const std::string &path, Request &req, const Server &server)
{
	if (access(path.c_str(), F_OK) == -1)
	{
		req.setStatus(NOT_FOUND);
		generateErrorPage(req, server);
		return _body;
	}
	if (access(path.c_str(), R_OK) == -1)
	{
		req.setStatus(FORBIDDEN);
		generateErrorPage(req, server);
		return _body;
	}

	std::ifstream ifs(path.c_str());

	return std::string(
		std::istreambuf_iterator<char>(ifs),
		std::istreambuf_iterator<char>()
	);
}

void Response::GET(Request &req, const Server &server, const client_info &client)
{
	Location *location = matchUri(req.uri(), server);

	if (location == NULL)
	{
		error(NOT_FOUND);
		return;
	}
	std::string path = location->root + req.uri();

	const std::string extension = Utils::getExtension(req.uri());
	std::map<std::string, std::string>::iterator	it =  location->cgi.find(extension);
	if (it != location->cgi.end())
	{
		try {
			Cgi	cgi(it->second);
			cgi.run(req, server, client, path);
			_contentLength = cgi.getContentLength();
			_contentType = cgi.getContentType();
			_status = cgi.getStatus();
			_body = cgi.getBody();
			return ;
		} catch (int &status) {
			_status = status;
			return ;
		}
	}
	_status = OK;
	if (Utils::isDirectory(path))
	{
		typedef std::set<std::string>::const_iterator setIterator;

		for (setIterator it = location->index.begin(); it != location->index.end(); ++it)
		{
			std::string indexPath = path + '/' + *it;

			if (access(indexPath.c_str(), F_OK) != -1)
			{
				_body = getFileContent(indexPath, req, server);
				_contentType = MimeTypes::getMimeType( Utils::getExtension(indexPath) );
				_contentLength = _body.length();
				break;
			}
		}
		if (_body.empty())
		{
			if (location->autoIndex)
			{
				_body = generateDirectoryListing(path, req.uri());
				_contentType = "text/html";
				_contentLength = _body.length();
			}
			else
			{
				error(FORBIDDEN, *location, server);
			}
		}
	}
	else // path refers to a file
	{
		_contentType = MimeTypes::getMimeType( Utils::getExtension(path) );
		_body = getFileContent(path, req, server);
		_contentLength = _body.length();
	}
}

void Response::DELETE(Request &req, const Server &server)
{
	Location *location = matchUri(req.uri(), server);
	if (location == NULL)
	{
		req.setStatus(NOT_FOUND);
		generateErrorPage(req, server);
		return;
	}
	std::string path = location->root + req.uri();

	if (access(path.c_str(), F_OK) == -1)
	{
		req.setStatus(NOT_FOUND);
		generateErrorPage(req, server);
		return;
	}
	if (access(path.c_str(), W_OK) == -1)
	{
		req.setStatus(FORBIDDEN);
		generateErrorPage(req, server);
		return;
	}
	if (Utils::isDirectory(path))
	{
		req.setStatus(FORBIDDEN);
		generateErrorPage(req, server);
		return;
	}
	if (unlink(path.c_str()) == -1)
	{
		req.setStatus(INTERNAL_SERVER_ERROR);
		generateErrorPage(req, server);
		return;
	}
	_status = NO_CONTENT;
}

void Response::PUT(Request &req, const Server &server)
{
	Location *location = matchUri(req.uri(), server);
	if (location == NULL)
	{
		req.setStatus(NOT_FOUND);
		generateErrorPage(req, server);
		return;
	}
	std::string path = location->root + req.uri();
	if (location->upload)
	{
		path = location->uploadPath + req.uri();
	}

	if (access(path.c_str(), F_OK) != -1)
	{
		_status = NO_CONTENT;
	}
	else
	{
		_status = CREATED;
	}
	if (Utils::isDirectory(path))
	{
		_status = CONFLICT;
		req.setStatus(CONFLICT);
		generateErrorPage(req, server);
		return;
	}
	std::ofstream ofs(path.c_str(), std::ios::binary);
	if (not ofs.is_open())
	{
		req.setStatus(INTERNAL_SERVER_ERROR);
		generateErrorPage(req, server);
		return;
	}
	for (size_t i = 0; i < req.body().length(); ++i)
	{
		ofs << req.body()[i];
	}
	ofs.close();
}
