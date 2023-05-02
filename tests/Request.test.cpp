#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Request.hpp"

TEST_CASE("Valid requests", "[Request]")
{
	{
		Request req;

		REQUIRE(req.readRequest("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"));
	}
	{
		Request req;

		REQUIRE(req.readRequest("DELETE /file.txt HTTP/1.1\nHost: localhost\n\n"));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST /users HTTP/1.1\r\n"));
		REQUIRE(req.readRequest("Host: localhost\r\n"));
		REQUIRE(req.readRequest("Content-Type: application/json\r\n"));
		REQUIRE(req.readRequest("Content-Length: 13\r\n"));
		REQUIRE(req.readRequest("\r\n"));
		REQUIRE(req.readRequest("{\"name\":\"John\"}"));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n"));
		REQUIRE(req.readRequest("Host: localhost\r\n"));
		REQUIRE(req.readRequest("Content-Type: text/plain\r\n"));
		REQUIRE(req.readRequest("Transfer-Encoding: chunked\r\n"));
		REQUIRE(req.readRequest("\r\n"));
		REQUIRE(req.readRequest("4\r\n"));
		REQUIRE(req.readRequest("Wiki\r\n"));
		REQUIRE(req.readRequest("5\r\n"));
		REQUIRE(req.readRequest("pedia\r\n"));
		REQUIRE(req.readRequest("E\r\n"));
		REQUIRE(req.readRequest(" in\r\n\r\nchunks.\r\n"));
		REQUIRE(req.readRequest("0\r\n"));
		REQUIRE(req.readRequest("\r\n"));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST"));
		REQUIRE(req.readRequest(" / HTTP/1.1\r\n"));
	}
}

TEST_CASE("Invalid requests", "[Request]")
{
	{
		Request req;

		REQUIRE_FALSE(req.readRequest("GET / HTTP/2.0"));
	}
	{
		Request req;

		REQUIRE_FALSE(req.readRequest(" GET / HTTP/1.1\r\n"));
	}
	{
		Request req;

		REQUIRE_FALSE(req.readRequest("GET / HTTP/1.1\r\nHost : localhost\r\n"));
	}
	{
		Request req;

		REQUIRE_FALSE(req.readRequest("UNKOWN / HTTP/1.1\r\nHost: localhost\r\n"));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n"));
		REQUIRE(req.readRequest("Host: localhost\r\n"));
		REQUIRE(req.readRequest("Content-Type: text/plain\r\n"));
		REQUIRE(req.readRequest("Content-Length: 13\r\n"));
		REQUIRE(req.readRequest("\r\n"));
		REQUIRE_FALSE(req.readRequest("not 13 characters\r\n"));
	}
	{
		Request req;

		REQUIRE(req.readRequest("POST / HTTP/1.1\r\n"));
		REQUIRE(req.readRequest("Host: localhost\r\n"));
		REQUIRE(req.readRequest("Content-Type: text/plain\r\n"));
		REQUIRE(req.readRequest("Transfer-Encoding: chunked\r\n"));
		REQUIRE(req.readRequest("\r\n"));
		REQUIRE(req.readRequest("4\r\n"));
		REQUIRE_FALSE(req.readRequest("More than 4 characters\r\n"));
	}
}
