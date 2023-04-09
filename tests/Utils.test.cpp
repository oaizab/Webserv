#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "Utils.hpp"

TEST_CASE("Trim", "[Utils]")
{
	REQUIRE(Utils::Trim("  hello world  ") == "hello world");
	REQUIRE(Utils::Trim("hello world  ") == "hello world");
	REQUIRE(Utils::Trim("  hello world") == "hello world");
	REQUIRE(Utils::Trim("hello world") == "hello world");
	REQUIRE(Utils::Trim("  ").empty() == true);
	REQUIRE(Utils::Trim("").empty() == true);
}
