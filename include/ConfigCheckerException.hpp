#pragma once

#include <exception>
#include <string>

class ConfigCheckerException : public std::exception
{
	public:
		explicit ConfigCheckerException(const char* message);
		const char* what() const throw();

	private:
		const char *message;
};
