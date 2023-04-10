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

		// One-line directive validators
		void validateHostDirective(const std::vector<std::string> &tokens);
		void validatePortDirective(const std::vector<std::string> &tokens);
		void validateServerNameDirective(const std::vector<std::string> &tokens);
		void validateErrorPagesDirective(const std::vector<std::string> &tokens, std::ifstream &fin);
		void validateClientMaxBodySizeDirective(const std::vector<std::string> &tokens);

		// TODO(ConfigChecker): Implement these One-line directive validators
		void validateAllowedMethodsDirective(const std::vector<std::string> &tokens);
		void validateRedirectDirective(const std::vector<std::string> &tokens);
		void validateRootDirective(const std::vector<std::string> &tokens);
		void validateAutoindexDirective(const std::vector<std::string> &tokens);
		void validateIndexDirective(const std::vector<std::string> &tokens);
		void validateUploadDirective(const std::vector<std::string> &tokens);

		// Block directive validators
		void validateServerBlock(std::ifstream &fin);
		void validateLocationDirective(const std::vector<std::string> &tokens, std::ifstream &fin);
		void validateCgiDirective(const std::vector<std::string> &tokens);

		// Methods
		void validateConfigFile();

	private:
		// Data members
		std::string configFilePath;
		std::ifstream fin;
};
