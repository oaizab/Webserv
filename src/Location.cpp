#include "Location.hpp"

Location::Location()
{
	autoIndex = false;
	upload = false;
	redirect = false;
}

void Location::addIndex(const std::string &index)
{
	this->index.push_back(index);
}

void Location::addAllowedMethod(const std::string &method)
{
	allowedMethods.push_back(method);
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

bool Location::isAutoIndex() const
{
	return autoIndex;
}

bool Location::isRedirection() const
{
	return redirect;
}
