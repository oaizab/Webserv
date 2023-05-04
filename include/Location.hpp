#pragma once

#include <set>
#include <string>
#include <map>

class Location
{
	public:
		// Constructors
		Location();

		// Methods
		void addIndex(const std::string &index);
		void addAllowedMethod(const std::string &method);
		void setRoot(const std::string &root);
		void createRedirection(const std::string &url, const std::string &code);
		void addCgi(const std::string &extension, const std::string &path);

		// Attributes
		std::string uri;
		std::string root;
		std::set<std::string> index;
		bool autoIndex;
		std::set<std::string> allowedMethods;
		bool upload;
		std::string uploadPath;
		bool redirect;
		std::string redirectUrl;
		std::string redirectCode;
		std::map<std::string, std::string> cgi;
};
