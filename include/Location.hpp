#pragma once

#include <set>
#include <string>

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
};
