#pragma once

#include <map>
#include <string>

class Request
{
	public:
		Request();
		~Request() {};

		bool readRequest(const std::string &request);

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
		bool _keepAlive;
		std::map<std::string, std::string> _headers;

		std::string _chunkSizeStr;
		size_t _chunkSize;
		std::string _chunkContent;
		bool _chunkSizeParsed;

		bool parseStartLine(const std::string &line);
		bool parseHeader(const std::string &line);
		bool parseBody(const std::string &line);
		bool parseHost(const std::string &line);
		bool parseUri(const std::string &line);
		bool parseMethod(const std::string &line);
		bool parseVersion(const std::string &line);
};