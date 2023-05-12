#include "Request.hpp"
#include "Http.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "statusCodes.hpp"
#include <algorithm>
#include <cctype>
#include <string>
#include <sys/_types/_size_t.h>

#include <iostream>

Request::Request()
{
	_state = START_LINE;
	_isHostParsed = false;
	_isStartLineParsed = false;
	_chunked = false;
	_contentLength = 0;
	_isContentLengthParsed = false;
	_chunkSize = 0;
	_chunkSizeParsed = false;
	_status = 0;
	_clientMaxBodySize = -1;
}

bool Request::readRequest(const std::string &request, int socketfd)
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
			if (not parseHeader(*it, _clientMaxBodySize))
				return false;
			if (_state == BODY)
			{
				Server &server = Http::matchHost(_host, socketfd);
				_clientMaxBodySize = server.clientMaxBodySize;
				if (_contentLength > _clientMaxBodySize)
				{
					_status = PAYLOAD_TOO_LARGE;
					return false;
				}
			}
		}
		else if (_state == BODY)
		{
			if (not parseBody(*it, _clientMaxBodySize))
				return false;
		}
	}
	return true;
}

bool Request::parseStartLine(const std::string &line)
{
	std::string tmp = line;
	if (tmp.empty())
	{
		return true;
	}
	if (tmp.front() == ' ' or tmp.front() == '\t')
	{
		_status = BAD_REQUEST;
		return false;
	}
	std::vector<std::string> tokens = Utils::Split(tmp, ' ');
	if (tokens.size() != 3)
	{
		_status = BAD_REQUEST;
		return false;
	}
	if (not parseMethod(tokens[0]))
		return false;
	if (not parseUri(tokens[1]))
	{
		_status = BAD_REQUEST;
		return false;
	}
	if (not parseVersion(tokens[2]))
	{
		_status = HTTP_VERSION_NOT_SUPPORTED;
		return false;
	}
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
	else if (line == "PUT")
		_method = "PUT";
	else if (line == "DELETE")
		_method = "DELETE";
	else
	{
		_status = NOT_IMPLEMENTED;
		return false;
	}
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

bool Request::parseBody(const std::string &line, size_t clientMaxBodySize)
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
				if (_body.length() > clientMaxBodySize)
				{
					_status = PAYLOAD_TOO_LARGE;
					return false;
				}
				_chunkSizeParsed = false;
				_chunkContent.clear();
			}
			if (_chunkSize == 0)
			{
				_status = OK;
				return false;
			}
		}
	}
	else
	{
		_body.append(line);

		if (_body.length() == _contentLength + 1 and _body.back() == '\n')
		{
			_body.pop_back();
			_status = OK;
			return false;
		}
		if (_body.length() == _contentLength + 2 and Utils::endsWith(_body, "\r\n"))
		{
			_body.erase(_body.length() - 2);
			_status = OK;
			return false;
		}

		if (_body.length() > _contentLength)
		{
			_status = BAD_REQUEST;
			return false;
		}
		if (_body.length() == _contentLength)
		{
			_status = OK;
			return false;
		}
	}
	return true;
}

bool Request::parseHeader(const std::string &line, size_t clientMaxBodySize)
{
	if (line.empty())
	{
		if (_method == "POST" or _method == "PUT")
		{
			if (_isContentLengthParsed and _contentLength == 0)
			{
				if (_isHostParsed)
					_status = OK;
				else
					_status = BAD_REQUEST;
				return false;
			}
			if (not _chunked and not _isContentLengthParsed)
			{
				if (not _isHostParsed)
				{
					_status = BAD_REQUEST;
					return false;
				}
				_status = LENGTH_REQUIRED;
				return false;
			}
			_state = BODY;
			return _isHostParsed;
		}
		if (_isHostParsed)
			_status = OK;
		else
			_status = BAD_REQUEST;
		return false;
	}
	std::vector<std::string> tokens = Utils::headerSplit(line);
	std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);
	if (tokens.size() != 2)
	{
		_status = BAD_REQUEST;
		return false;
	}
	if (tokens[0] == "host")
	{
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		std::string val = Utils::Trim(tokens[1]);
		size_t pos = val.find(':');
		if (pos != std::string::npos)
			val = val.substr(0, pos);
		if (_isHostParsed)
		{
			_status = BAD_REQUEST;
			return false;
		}
		_host = val;
		_isHostParsed = true;
	}
	else if (tokens[0] == "content-length")
	{
		if (_chunked)
		{
			_status = BAD_REQUEST;
			return false;
		}
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		std::string val = Utils::Trim(tokens[1]);
		if (_isContentLengthParsed)
		{
			_status = BAD_REQUEST;
			return false;
		}
		if (val.empty() or val.find_first_not_of("0123456789") != std::string::npos)
		{
			_status = BAD_REQUEST;
			return false;
		}
		_contentLength = std::stoi(val);
		if (_contentLength > clientMaxBodySize)
		{
			_status = PAYLOAD_TOO_LARGE;
			return false;
		}
		_isContentLengthParsed = true;
	}
	else if (tokens[0] == "transfer-encoding")
	{
		if (_isContentLengthParsed)
		{
			_status = BAD_REQUEST;
			return false;
		}
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		std::string val = tokens[1];
		val = Utils::Trim(val);
		if (val == "chunked")
			_chunked = true;
		else
		{
			_status = BAD_REQUEST;
			return false;
		}
	}
	else if (tokens[0] == "content-type")
	{
		std::replace(tokens[1].begin(), tokens[1].end(), '\t', ' ');
		tokens[1] = Utils::Trim(tokens[1]);
		_contentType = tokens[1];
	}
	else
	{
		if (tokens[0].find(' ') != std::string::npos or tokens[0].find('\t') != std::string::npos)
		{
			_status = BAD_REQUEST;
			return false;
		}
	}
	return true;
}

const std::string &Request::method() const
{
	return _method;
}

const std::string &Request::uri() const
{
	return _uri;
}

const std::string &Request::body() const
{
	return _body;
}

const std::string &Request::host() const
{
	return _host;
}

int Request::status() const
{
	return _status;
}

void Request::setStatus(int status)
{
	_status = status;
}

const std::string &Request::contentType() const
{
	return _contentType;
}
