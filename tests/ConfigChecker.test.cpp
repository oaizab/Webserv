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
	REQUIRE(ConfigChecker::validateHostname("unknow") == false);
}

TEST_CASE("validateServerName", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validateServerName("localhost") == true);
	REQUIRE(ConfigChecker::validateServerName("www.google.com") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validateServerName("") == false);
	REQUIRE(ConfigChecker::validateServerName(std::string(255, 'A')) == false);
	REQUIRE(ConfigChecker::validateServerName("-thisShouldBeInvalid") == false);
	REQUIRE(ConfigChecker::validateServerName("thisAlsoShouldBeInvalid-") == false);
	REQUIRE(ConfigChecker::validateServerName("www.google.com.") == false);
	REQUIRE(ConfigChecker::validateServerName("ho$tn@me-with-inv@lid-ch@r@cter$") == false);
	REQUIRE(ConfigChecker::validateServerName(std::string(64, 'A').append(".com")) == false);
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
	REQUIRE(ConfigChecker::validateSize("0k") == true);
	REQUIRE(ConfigChecker::validateSize("0K") == true);
	REQUIRE(ConfigChecker::validateSize("0m") == true);
	REQUIRE(ConfigChecker::validateSize("0M") == true);

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

TEST_CASE("validateErrorCode", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validateErrorCode("300") == true);
	REQUIRE(ConfigChecker::validateErrorCode("599") == true);
	REQUIRE(ConfigChecker::validateErrorCode("404") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validateErrorCode("") == false);
	REQUIRE(ConfigChecker::validateErrorCode("600") == false);
	REQUIRE(ConfigChecker::validateErrorCode("200") == false);
	REQUIRE(ConfigChecker::validateErrorCode("-1") == false);
	REQUIRE(ConfigChecker::validateErrorCode("notAnErrorCode") == false);
}

TEST_CASE("validateErrorPages", "[ConfigChecker]")
{
	// Valid testcases
	REQUIRE(ConfigChecker::validateErrorPages("404 /404.html") == true);
	REQUIRE(ConfigChecker::validateErrorPages("404 403 /notfound.php") == true);
	REQUIRE(ConfigChecker::validateErrorPages("404 403 400 401 /notfound.php") == true);

	// Invalid testcases
	REQUIRE(ConfigChecker::validateErrorPages("") == false);
	REQUIRE(ConfigChecker::validateErrorPages("notAStatusCode /404.html") == false);
	REQUIRE(ConfigChecker::validateErrorPages("404 notAStatusCode /404.html") == false);
	REQUIRE(ConfigChecker::validateErrorPages("/404.html 404") == false);
	REQUIRE(ConfigChecker::validateErrorPages("404 403 /notfound.php /anotherfile.php") == false);
}

TEST_CASE("validateErrorPagesDirective", "[ConfigChecker]")
{
	// Valid testcase #1 (Regular)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten 80" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_NOTHROW(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Valid testcase #2 (Empty server block)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl;
		file << "}" << std::endl;

		file.close();
		REQUIRE_NOTHROW(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Valid testcase #3 (Empty error_pages directive)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl;
		file << "\terror_pages {" << std::endl;
		file << "\t}" << std::endl;
		file << "}" << std::endl;

		file.close();
		REQUIRE_NOTHROW(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Valid testcase #4 (Normal config with location block)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost:8080" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "\n" << std::endl
			 << "\tlocation / {" << std::endl
			 << "\t\troot /var/www" << std::endl
			 << "\t\tindex index.html index.php" << std::endl
			 << "\t}" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_NOTHROW(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Valid testcase #5 (Config with multiple location blocks)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "\n" << std::endl
			 << "\tlocation / {" << std::endl
			 << "\t\troot /var/www" << std::endl
			 << "\t\tindex index.html index.php" << std::endl
			 << "\t}" << std::endl
			 << "\n" << std::endl
			 << "\tlocation /api {" << std::endl
			 << "\t\troot /var/www/api" << std::endl
			 << "\t\tindex index.html index.php" << std::endl
			 << "\t}" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_NOTHROW(ConfigChecker("test.conf"));
		remove("test.conf");
	}

	// Invalid testcase #1 (Missing closing curly brace)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Invalid testcase #2 (Wrong listen directive)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Invalid testcase #3 (Duplicate client_max_body_size)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost:8080" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Invalid testcase #4 (Missing closing brace for server)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost:8080" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Invalid testcase #5 (Duplicate error_pages directive)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost:8080" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Invalid testcase #6 (Empty file)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Invalid testcase #7 (Location block with no closing brace)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost:8080" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "\n" << std::endl
			 << "\tlocation / {" << std::endl
			 << "\t\troot /var/www" << std::endl
			 << "\t\tindex index.html index.php" << std::endl
			 << "\t}" << std::endl
			 << "\n" << std::endl
			 << "\tlocation /api {" << std::endl
			 << "\t\troot /var/www/api" << std::endl
			 << "\t\tindex index.html index.php" << std::endl
			 << "\tautoindex on" << std::endl
			 << "\tupload off" << std::endl
			 << "\t}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Valid testcase #8 (Index with no files specified)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost:8080" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "\n" << std::endl
			 << "\terror_pages {" << std::endl
			 << "\t\t404 /404.html" << std::endl
			 << "\t\t500 501 502 /50x.html" << std::endl
			 << "\t\t301 307 /redirect.php" << std::endl
			 << "\t}" << std::endl
			 << "\tclient_max_body_size 42m" << std::endl
			 << "\n" << std::endl
			 << "\tlocation / {" << std::endl
			 << "\t\troot /var/www" << std::endl
			 << "\t\tindex" << std::endl
			 << "\t}" << std::endl
			 << "\n" << std::endl
			 << "\tlocation /api {" << std::endl
			 << "\t\troot /var/www/api" << std::endl
			 << "\t\tindex" << std::endl
			 << "\tautoindex on" << std::endl
			 << "\tupload off" << std::endl
			 << "\t}" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Wrong testcase #9 (Invalid hostname on listen)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten not-localhost:8080" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
	// Wrong testcase #10 (Invalid port number on listen)
	{
		std::fstream file("test.conf", std::ios::out | std::ios::trunc);

		file << "server {" << std::endl
			 << "\tlisten localhost:80800" << std::endl
			 << "\tserver_name webserv" << std::endl
			 << "}" << std::endl;

		file.close();
		REQUIRE_THROWS(ConfigChecker("test.conf"));
		remove("test.conf");
	}
}
