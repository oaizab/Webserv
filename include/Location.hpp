#pragma once

#include <string>
#include <vector>

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

		bool isAutoIndex() const;
		bool isRedirection() const;
		

		// Attributes
		std::string root;
		std::vector<std::string> index;
		bool autoIndex;
		std::vector<std::string> allowedMethods;
		bool upload;
		std::string uploadPath;
		bool redirect;
		std::string redirectUrl;
		std::string redirectCode;
};
