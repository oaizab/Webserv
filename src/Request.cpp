#include "Request.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <cctype>
#include <string>

Request::Request()
{
	_state = START_LINE;
	_isHostParsed = false;
	_isStartLineParsed = false;
	_chunked = false;
	_contentLength = 0;
	_isContentLengthParsed = false;
	_keepAlive = false;
	_chunkSize = 0;
	_chunkSizeParsed = false;
	_status = 0;
}

bool Request::readRequest(const std::string &request)
{
	_request += request;
	std::vector<std::string> lines = Utils::reqSplit(_request);
	_request.clear();
	if (lines.empty() and (not _isStartLineParsed or not _isHostParsed))
		return false;
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		if (it->back() != '\n' and _state != BODY)
		{
			_request = *it;
			break;
		}
		if (_state != BODY)
		{
			if (it->back() == '\n')
				it->pop_back();
			if (it->back() == '\r')
				it->pop_back();
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
	if (tokens.size() != 3)
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
	return line == "HTTP/1.1";
}

bool Request::parseBody(const std::string &line)
{
	if (_chunked)
	{
		if (not _chunkSizeParsed)
		{
			_chunkSizeStr.append(line);
			if (Utils::endsWith(_chunkSizeStr, "\n"))
			{
				_chunkSizeStr.pop_back();
				if (Utils::endsWith(_chunkSizeStr, "\r"))
					_chunkSizeStr.pop_back();
				if (
					_chunkSizeStr.empty()
					or _chunkSizeStr.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos
				)
				{
					_status = BAD_REQUEST;
					return false;
				}
				_chunkSize = std::stoul(_chunkSizeStr, NULL, 16);
				_chunkSizeStr.clear();
				_chunkSizeParsed = true;
			}
		}
		else
		{
			uint8_t prefixLength = 0;

			_chunkContent.append(line);
			if (Utils::endsWith(_chunkContent, "\r\n"))
				prefixLength = 2;
			else if (Utils::endsWith(_chunkContent, "\n"))
				prefixLength = 1;
			else
				return true;
			if (_chunkContent.length() > _chunkSize + prefixLength)
			{
				_status = BAD_REQUEST;
				return false;
			}
			if (_chunkContent.length() == _chunkSize + prefixLength)
			{
				_chunkContent.erase(_chunkContent.length() - prefixLength); // Remove CRLF or LF
				_body.append(_chunkContent);
				_chunkSize = 0;
				_chunkSizeParsed = false;
				_chunkContent.clear();
			}
		}
	}
	else
	{
		_body.append(line);
		if (_body.length() > _contentLength)
		{
			_status = BAD_REQUEST;
			return false;
		}
	}
	return true;
}

bool Request::parseHeader(const std::string &line)
{
	if (line.empty())
	{
		_state = BODY;
		return _isHostParsed;
	}
	std::vector<std::string> tokens = Utils::Split(line, ':');
	if (tokens.size() != 2)
		return false;
	std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);
	if (tokens[0] == "host")
	{
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		std::string val = Utils::Trim(tokens[1]);
		if (_isHostParsed)
			return false;
		_host = val;
		_isHostParsed = true;
	}
	else if (tokens[0] == "content-length")
	{
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		std::string val = Utils::Trim(tokens[1]);
		if (_isContentLengthParsed)
			return false;
		if (val.empty() or val.find_first_not_of("0123456789") != std::string::npos)
			return false;
		_contentLength = std::stoi(val);
		_isContentLengthParsed = true;
	}
	else if (tokens[0] == "transfer-encoding")
	{
		if (_isContentLengthParsed)
			return false;
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		std::string val = tokens[1];
		val = Utils::Trim(val);
		if (val == "chunked")
			_chunked = true;
		else
			return false;
	}
	else if (tokens[0] == "connection")
	{
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		std::string val = Utils::Trim(tokens[1]);
		if (val == "close")
			_keepAlive = false;
		else if (val == "keep-alive")
			_keepAlive = true;
		else
			return false;
	}
	else
	{
		if (tokens[0].find(' ') != std::string::npos or tokens[0].find('\t') != std::string::npos)
			return false;
	}
	return true;
}
