#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Request.hpp"

TEST_CASE("Valid requests", "[Request]")
{
	const size_t CLIENT_MAX_BODY_SIZE = 1024 * 1024;
	{
		Request req;

		REQUIRE_FALSE(req.readRequest("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE_FALSE(req.readRequest("DELETE /file.txt HTTP/1.1\nHost: localhost\n\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST /users HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Host: localhost\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Content-Type: application/json\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Content-Length: 15\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("{\"name\":\"John\"}", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Host: localhost\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Content-Type: text/plain\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Transfer-Encoding: chunked   \r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("4\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Wiki\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("5\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("pedia\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("E\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest(" in\r\n\r\nchunks.\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("0\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest(" / HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
	}
}

TEST_CASE("Invalid requests", "[Request]")
{
	const size_t CLIENT_MAX_BODY_SIZE = 1024 * 1024;

	{
		Request req;

		REQUIRE(req.readRequest("GET / HTTP/2.0", CLIENT_MAX_BODY_SIZE));
		REQUIRE_FALSE(req.readRequest("\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE_FALSE(req.readRequest(" GET / HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE_FALSE(req.readRequest("GET / HTTP/1.1\r\nHost : localhost\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE_FALSE(req.readRequest("UNKOWN / HTTP/1.1\r\nHost: localhost\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Host: localhost\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Content-Type: text/plain\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Content-Length: 13\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE_FALSE(req.readRequest("not 13 characters\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Host: localhost\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Content-Type: text/plain\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Transfer-Encoding: chunked\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("4\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE_FALSE(req.readRequest("More than 4 characters\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Host: localhost\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE_FALSE(req.readRequest("Content-Length: invalidLength\r\n", CLIENT_MAX_BODY_SIZE));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE(req.readRequest("Host: localhost\r\n", CLIENT_MAX_BODY_SIZE));
		REQUIRE_FALSE(req.readRequest("Content-Length: \r\n", CLIENT_MAX_BODY_SIZE));
	}
}
