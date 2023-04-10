#include "Location.hpp"

Location::Location()
{
	autoIndex = false;
	upload = false;
	redirect = false;
}

void Location::addIndex(const std::string &index)
{
	this->index.insert(index);
}

void Location::addAllowedMethod(const std::string &method)
{
	allowedMethods.insert(method);
}

void Location::setRoot(const std::string &root)
{
	this->root = root;
}

void Location::createRedirection(const std::string &url, const std::string &code)
{
	if (!redirect)
	{
		redirect = true;
		redirectUrl = url;
		redirectCode = code;
	}
}

