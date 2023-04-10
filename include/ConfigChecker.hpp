#pragma once

#include <cstring>
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <iostream>

class ConfigChecker
{
	public:
		// Constructors
		ConfigChecker();
		explicit ConfigChecker(const std::string &configFilePath);

		// Destructor
		~ConfigChecker();

		// Static methods
		static bool validateIp(const std::string &ipAddress);
		static bool validatePortNumber(const std::string &port);
		static bool validateHostname(const std::string &hostname);
		static bool validateSize(const std::string &sizeStr);
		static bool validateHttpMethod(const std::string &method);
		static bool validateErrorCode(const std::string &code);
		static bool validateErrorPages(const std::string &errorPages);

		static void validateServerBlock(std::ifstream &fin);

		static void validateHostDirective(const std::vector<std::string> &tokens);
		static void validatePortDirective(const std::vector<std::string> &tokens);
		static void validateServerNameDirective(const std::vector<std::string> &tokens);
		static void validateErrorPagesDirective(const std::vector<std::string> &tokens, std::ifstream &fin);
		static void validateClientMaxBodySizeDirective(const std::vector<std::string> &tokens);

		static void validateLocationDirective(const std::vector<std::string> &tokens, std::ifstream &fin);
		static void validateAllowedMethodsDirective(const std::vector<std::string> &tokens);
		static void validateRedirectDirective(const std::vector<std::string> &tokens);
		static void validateRootDirective(const std::vector<std::string> &tokens);
		static void validateAutoindexDirective(const std::vector<std::string> &tokens);
		static void validateIndexDirective(const std::vector<std::string> &tokens);
		static void validateCgiDirective(const std::vector<std::string> &tokens);
		static void validateUploadDirective(const std::vector<std::string> &tokens);

		// Methods
		void validateConfigFile();

	private:
		// Data members
		std::string configFilePath;
		std::ifstream fin;
};
