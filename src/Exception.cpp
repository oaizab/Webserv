#include "Exception.hpp"

ConfigCheckerException::ConfigCheckerException(const std::string &message) : message(message) {}

ConfigCheckerException::~ConfigCheckerException() throw() {}

const char *ConfigCheckerException::what() const throw()
{
	return message.c_str();
}


FileException::FileException(const std::string &message) : message(message) {}

FileException::~FileException() throw() {}

const char *FileException::what() const throw()
{
	return message.c_str();
}
