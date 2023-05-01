#include "Request.hpp"
#include "Utils.hpp"
#include <sys/_types/_size_t.h>

Request::Request()
{
	_state = START_LINE;
	_isHostParsed = false;
	_isStartLineParsed = false;
	_chuncked = false;
	_contentLength = 0;
	_isContentLengthParsed = false;
}

bool Request::readRequest(const std::string &request)
{
	_request += request;
	std::vector<std::string> lines = Utils::reqSplit(_request);
	if (lines.empty() and (not _isStartLineParsed or not _isHostParsed))
		return false;
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if (it->back() != '\n' and _state != BODY)
		{
			_request = *it;
			break;
		}
		if (_state == START_LINE)
		{
			if (not parseStartLine(*it))
				return false;
		}
		else if (_state == HEADER)
		{
			if (not parseHeader(*it))
				return false;
		}
		else if (_state == BODY)
		{
			if (not parseBody(*it))
				return false;
		}
	}
	return true;
}

bool Request::parseStartLine(const std::string &line)
{
	std::string tmp = line;
	if (tmp.front() == ' ')
		return false;
	std::vector<std::string> tokens = Utils::Split(tmp, ' ');
	if (tokens.size() != 3 or (tokens.size() == 4 and (tokens.back() == "\r\n" or tokens.back() == "\n")))
		return false;
	if (not parseMethod(tokens[0]))
		return false;
	if (not parseUri(tokens[1]))
		return false;
	if (not parseVersion(tokens[2]))
		return false;
	_state = HEADER;
	_isStartLineParsed = true;
	return true;
}

bool Request::parseMethod(const std::string &line)
{
	if (line == "GET")
		_method = "GET";
	else if (line == "POST")
		_method = "POST";
	else if (line == "DELETE")
		_method = "DELETE";
	else
		return false;
	return true;
}

bool Request::parseUri(const std::string &line)
{
	if (line.front() != '/')
		return false;
	if (line.find("/..") != std::string::npos)
		return false;
	size_t pos = line.find('?');
	if (pos != std::string::npos)
	{
		_uri = line.substr(0, pos);
		_query = line.substr(pos + 1);
	}
	else
		_uri = line;
	return true;
}

bool Request::parseVersion(const std::string &line)
{
	return line == "HTTP/1.1" or line == "HTTP/1.1\r\n" or line == "HTTP/1.1\n";
}

