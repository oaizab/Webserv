#pragma once

#include <exception>
#include <string>

class ConfigCheckerException : public std::exception
{
	public:
		explicit ConfigCheckerException(const std::string &message);

		~ConfigCheckerException() throw();

		virtual const char* what() const throw();

	private:
		std::string message;
};

class FileException : public std::exception
{
	public:
		explicit FileException(const std::string &message);
		const char* what() const throw();

		~FileException() throw();

	private:
		std::string message;
};
