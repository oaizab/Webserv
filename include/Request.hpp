#pragma once

#include <map>
#include <string>

#include "statusCodes.hpp"

class Request
{
	public:
		Request();
		~Request() {};

		bool readRequest(const std::string &request, int socketFd);

		const std::string &method() const;
		const std::string &uri() const;
		const std::string &body() const;
		const std::string &host() const;
		int status() const;
		void setStatus(int status);
		const std::string &contentType() const;

	private:
		enum State
		{
			START_LINE,
			HEADER,
			BODY
		};

		std::string _request;
		std::string _method;
		std::string _uri;
		std::string _query;
		std::string _body;
		std::string _host;
		State _state;
		bool _isStartLineParsed;
		bool _isHostParsed;
		bool _chunked;
		size_t _contentLength;
		bool _isContentLengthParsed;

		std::string _chunkSizeStr;
		size_t _chunkSize;
		std::string _chunkContent;
		bool _chunkSizeParsed;

		int _status;

		std::string _contentType;

		bool parseStartLine(const std::string &line);
		bool parseHeader(const std::string &line, size_t clientMaxBodySize);
		bool parseBody(const std::string &line, size_t clientMaxBodySize);
		bool parseHost(const std::string &line);
		bool parseUri(const std::string &line);
		bool parseMethod(const std::string &line);
		bool parseVersion(const std::string &line);
};
