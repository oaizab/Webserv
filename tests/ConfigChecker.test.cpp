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
