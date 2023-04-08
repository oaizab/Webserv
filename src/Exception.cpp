#include "Exception.hpp"

ConfigCheckerException::ConfigCheckerException(const char *message) : message(message) {}

const char *ConfigCheckerException::what() const throw()
{
	return message;
}

FileException::FileException(const char *message) : message(message) {}

const char *FileException::what() const throw()
{
	return message;
}
