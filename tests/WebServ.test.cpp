#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "WebServ.hpp"

TEST_CASE("parseClientMaxBodySizeParam", "[WebServ]")
{
	// valid testcases
	REQUIRE(WebServ::parseClientMaxBodySizeParam("1") == 1ULL);
	REQUIRE(WebServ::parseClientMaxBodySizeParam("1k") == 1024ULL);
	REQUIRE(WebServ::parseClientMaxBodySizeParam("1m") == 1048576ULL);
	REQUIRE(WebServ::parseClientMaxBodySizeParam("5m") == 5242880ULL);
	REQUIRE(WebServ::parseClientMaxBodySizeParam("0m") == 0);
}
