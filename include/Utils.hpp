#pragma once

#include <string>
#include <vector>
#include <unistd.h>

namespace Utils
{
	std::string Trim(const std::string &str);
	std::vector<std::string> Split(const std::string &str, char delim);
	std::vector<std::string> reqSplit(const std::string &str);
	bool endsWith(const std::string &str, const std::string &suffix);
	bool startsWith(const std::string &str, const std::string &prefix);
	std::vector<std::string> headerSplit(const std::string &str);
	bool isDirectory(const std::string &path);
	bool isFile(const std::string &path);
	std::string getExtension(const std::string &path);
	std::string	getAbsolutePath(const std::string &filename);
};
