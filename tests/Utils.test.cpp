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

TEST_CASE("Split", "[Utils]")
{
	const std::string EXPECTED_ARRAY[] = { "hello", "world" };
	const std::vector<std::string> EXPECTED(EXPECTED_ARRAY, EXPECTED_ARRAY + 2);

	REQUIRE(Utils::Split("hello world", ' ') == EXPECTED);
	REQUIRE(Utils::Split("  hello world   ", ' ') == EXPECTED);
	REQUIRE(Utils::Split("hello      world", ' ') == EXPECTED);
	REQUIRE(Utils::Split("", ' ').empty() == true);
	REQUIRE(Utils::Split("     ", ' ').empty() == true);

	REQUIRE(Utils::Split("hello world", 'x').size() == 1);
	REQUIRE(Utils::Split("hello world", 'x').front() == "hello world");
}
