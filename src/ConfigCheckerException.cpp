#include "ConfigCheckerException.hpp"

ConfigCheckerException::ConfigCheckerException(const char *message) : message(message) {}

const char *ConfigCheckerException::what() const throw()
{
	return message;
}
