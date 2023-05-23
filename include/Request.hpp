#pragma once

#include <map>
#include <string>
#include <sys/_types/_size_t.h>
#include <vector>
#include "statusCodes.hpp"
#include "ByteSequence.hpp"
#define MAX_PORT 65535

class Request
{
	public:
		Request();
		~Request() {};

		bool readRequest(const ByteSequence &request, int socketFd);

		const std::string &method() const;
		const std::string &uri() const;
		const ByteSequence &body() const;
		const std::string &host() const;
		const std::string &port() const;
		const std::string &query() const;
		int status() const;
		void setStatus(int status);
		const std::string &contentType() const;
		size_t contentLength() const;

	private:
		enum State
		{
			START_LINE,
			HEADER,
			BODY
		};

		ByteSequence _request;
		std::string _method;
		std::string _uri;
		std::string _query;
		ByteSequence _body;
		std::string _host;
		std::string	_port;
		State _state;
		bool _isStartLineParsed;
		bool _isHostParsed;
		bool _chunked;
		size_t _contentLength;
		bool _isContentLengthParsed;

		std::string _chunkSizeStr;
		size_t _chunkSize;
		ByteSequence _chunkContent;
		bool _chunkSizeParsed;
		int _status;
		std::string _contentType;
		size_t _clientMaxBodySize;

		bool parseStartLine(const std::string &line);
		bool parseHeader(const std::string &line, size_t clientMaxBodySize);
		bool parseBody(const ByteSequence &line, size_t clientMaxBodySize);
		bool parseHost(const std::string &line);
		bool parseUri(const std::string &line);
		bool parseMethod(const std::string &line);
		bool parseVersion(const std::string &line);
};
