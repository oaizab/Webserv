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

TEST_CASE("parseLocationBlock", "[WebServ]")
{
	std::fstream file("test.conf", std::ios::out | std::ios::trunc);

	file << "\troot /some/path" << std::endl
		 << "\tindex index.html index.php" << std::endl
		 << "\tautoindex on" << std::endl
		 << "\tupload off" << std::endl
		 << "\tredirect 307 /some/other/route" << std::endl
		 << "\tallowed_methods GET POST" << std::endl
		 << "}" << std::endl;

	file.close();

	std::ifstream fin("test.conf");

	Location location = WebServ::parseLocationBlock(fin, "/someRoute");

	REQUIRE(location.uri == "/someRoute");
	REQUIRE(location.root == "/some/path");
	REQUIRE(location.index.find("index.html") != location.index.end());
	REQUIRE(location.index.find("index.php") != location.index.end());
	REQUIRE(location.autoIndex == true);
	REQUIRE(location.upload == true);
	REQUIRE(location.uploadPath == "off");
	REQUIRE(location.redirect == true);
	REQUIRE(location.redirectCode == "307");
	REQUIRE(location.redirectUrl == "/some/other/route");
	REQUIRE(location.allowedMethods.find("GET") != location.allowedMethods.end());
	REQUIRE(location.allowedMethods.find("POST") != location.allowedMethods.end());

	remove("test.conf");
}

TEST_CASE("parseServerBlock", "[WebServ]")
{
	std::fstream file("test.conf", std::ios::out | std::ios::trunc);

	file << "listen 127.0.0.1:8080" << std::endl
		 << "client_max_body_size 1m" << std::endl
		 << "error_pages {" << std::endl
		 << "\t500 501 502 /50x.html" << std::endl
		 << "\t404 /404.html" << std::endl
		 << "}" << std::endl
		 << "server_name webserv" << std::endl
		 << "location / {" << std::endl
		 << "\troot /var/www/html" << std::endl
		 << "\tautoindex off" << std::endl
		 << "\tindex index.html" << std::endl
		 << "\tupload /uploaded-data" << std::endl
		 << "\t}" << std::endl
		 << "}" << std::endl;

	file.close();

	std::ifstream fin("test.conf");

	Server server = WebServ::parseServerBlock(fin);

	REQUIRE(server.listen.size() == 1);

	std::pair<std::string, std::string> listenPair = server.listen[0];

	REQUIRE(listenPair.first == "127.0.0.1");
	REQUIRE(listenPair.second == "8080");
	REQUIRE(server.clientMaxBodySize == 1048576ULL);
	REQUIRE(server.errorPages["404"] == "/404.html");
	REQUIRE(server.errorPages["500"] == "/50x.html");
	REQUIRE(server.errorPages["501"] == "/50x.html");
	REQUIRE(server.errorPages["502"] == "/50x.html");
	REQUIRE(server.serverNames.size() == 1);
	REQUIRE(server.serverNames[0] == "webserv");
	REQUIRE(server.locations.size() == 1);
	REQUIRE(server.locations[0].uri == "/");
	REQUIRE(server.locations[0].root == "/var/www/html");
	REQUIRE(server.locations[0].index.find("index.html") != server.locations[0].index.end());
	REQUIRE(server.locations[0].autoIndex == false);
	REQUIRE(server.locations[0].upload == true);
	REQUIRE(server.locations[0].uploadPath == "/uploaded-data");

	remove("test.conf");
}
