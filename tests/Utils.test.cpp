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

TEST_CASE("reqSplit", "[Utils]")
{
	{
		const std::string EXPECTED_ARRAY[] = { "GET / HTTP/1.1\r\n", "Ho" };
		const std::vector<std::string> EXPECTED(EXPECTED_ARRAY, EXPECTED_ARRAY + 2);
		REQUIRE(Utils::reqSplit("GET / HTTP/1.1\r\nHo") == EXPECTED);
	}
	{
		const std::string EXPECTED_ARRAY[] = { "\n", "\n" };
		const std::vector<std::string> EXPECTED(EXPECTED_ARRAY, EXPECTED_ARRAY + 2);
		REQUIRE(Utils::reqSplit("\n\n") == EXPECTED);
	}
	{
		const std::string NOT_EXPECTED_ARRAY[] = { "GET / HTTP/1.1\r", "Ho" };
		const std::vector<std::string> NOT_EXPECTED(NOT_EXPECTED_ARRAY, NOT_EXPECTED_ARRAY + 2);
		REQUIRE(Utils::reqSplit("GET / HTTP/1.1\r\nHo") != NOT_EXPECTED);
	}
}

TEST_CASE("endsWith", "[Utils]")
{
	REQUIRE(Utils::endsWith("hello world", "world") == true);
	REQUIRE(Utils::endsWith("hello world", "world!") == false);
	REQUIRE(Utils::endsWith("hello world", "") == true);
}

TEST_CASE("startsWith", "[Utils]")
{
	REQUIRE(Utils::startsWith("hello world", "hello") == true);
	REQUIRE(Utils::startsWith("hello world", "hello!") == false);
	REQUIRE(Utils::startsWith("hello world", "") == true);
}
