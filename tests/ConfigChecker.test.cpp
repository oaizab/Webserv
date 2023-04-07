#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "ConfigChecker.hpp"

TEST_CASE("validateIp", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validateIp("127.0.0.1") == true);
	REQUIRE(ConfigChecker::validateIp("192.168.1.100") == true);
	REQUIRE(ConfigChecker::validateIp("0.0.0.0") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validateIp("") == false);
	REQUIRE(ConfigChecker::validateIp("notAnIpAddress") == false);
	REQUIRE(ConfigChecker::validateIp("127001") == false);
	REQUIRE(ConfigChecker::validateIp("127.0.0") == false);
	REQUIRE(ConfigChecker::validateIp("300.0.0.1") == false);
	REQUIRE(ConfigChecker::validateIp("-1.0.0.1") == false);
	REQUIRE(ConfigChecker::validateIp(" 127.0.0.1") == false);
	REQUIRE(ConfigChecker::validateIp("127. 0.0.1") == false);
}

TEST_CASE("validatePort", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validatePortNumber("1") == true);
	REQUIRE(ConfigChecker::validatePortNumber("80") == true);
	REQUIRE(ConfigChecker::validatePortNumber("8080") == true);
	REQUIRE(ConfigChecker::validatePortNumber("65535") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validatePortNumber("") == false);
	REQUIRE(ConfigChecker::validatePortNumber("notAPort") == false);
	REQUIRE(ConfigChecker::validatePortNumber("65536") == false);
	REQUIRE(ConfigChecker::validatePortNumber("0") == false);
	REQUIRE(ConfigChecker::validatePortNumber("-1") == false);
	REQUIRE(ConfigChecker::validatePortNumber(" 80") == false);
}

TEST_CASE("validateHostname", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validateHostname("localhost") == true);
	REQUIRE(ConfigChecker::validateHostname("www.google.com") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validateHostname("") == false);
	REQUIRE(ConfigChecker::validateHostname(std::string(255, 'A')) == false);
	REQUIRE(ConfigChecker::validateHostname("-thisShouldBeInvalid") == false);
	REQUIRE(ConfigChecker::validateHostname("thisAlsoShouldBeInvalid-") == false);
	REQUIRE(ConfigChecker::validateHostname("www.google.com.") == false);
	REQUIRE(ConfigChecker::validateHostname("ho$tn@me-with-inv@lid-ch@r@cter$") == false);
	REQUIRE(ConfigChecker::validateHostname(std::string(64, 'A').append(".com")) == false);
}

TEST_CASE("validateSize", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validateSize("0") == true);
	REQUIRE(ConfigChecker::validateSize("1") == true);
	REQUIRE(ConfigChecker::validateSize("1337") == true);
	REQUIRE(ConfigChecker::validateSize("13371337") == true);
	REQUIRE(ConfigChecker::validateSize("42k") == true);
	REQUIRE(ConfigChecker::validateSize("42K") == true);
	REQUIRE(ConfigChecker::validateSize("42m") == true);
	REQUIRE(ConfigChecker::validateSize("42M") == true);
	REQUIRE(ConfigChecker::validateSize("42g") == true);
	REQUIRE(ConfigChecker::validateSize("42G") == true);
	REQUIRE(ConfigChecker::validateSize("0k") == true);
	REQUIRE(ConfigChecker::validateSize("0K") == true);
	REQUIRE(ConfigChecker::validateSize("0m") == true);
	REQUIRE(ConfigChecker::validateSize("0M") == true);
	REQUIRE(ConfigChecker::validateSize("0g") == true);
	REQUIRE(ConfigChecker::validateSize("0G") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validateSize("") == false);
	REQUIRE(ConfigChecker::validateSize("notASize") == false);
	REQUIRE(ConfigChecker::validateSize("1k1") == false);
	REQUIRE(ConfigChecker::validateSize("-1") == false);
	REQUIRE(ConfigChecker::validateSize("1.1") == false);
	REQUIRE(ConfigChecker::validateSize("-1k") == false);
	REQUIRE(ConfigChecker::validateSize("1kg") == false);
}

TEST_CASE("validateHttpMethod", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validateHttpMethod("GET") == true);
	REQUIRE(ConfigChecker::validateHttpMethod("POST") == true);
	REQUIRE(ConfigChecker::validateHttpMethod("DELETE") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validateHttpMethod("") == false);
	REQUIRE(ConfigChecker::validateHttpMethod("notAMethod") == false);
}
