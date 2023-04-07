#include "ConfigChecker.hpp"

bool ConfigChecker::validateIp(const std::string &ipAddress)
{
	std::vector<int> components;
	std::stringstream stream(ipAddress);
	std::string component;

	while (std::getline(stream, component, '.'))
	{
		if (component.empty() or component.size() > 3)
		{
			return false;
		}
		if (std::strspn(component.c_str(), "0123456789") != component.size())
		{
			return false;
		}

		int value = std::stoi(component);

		if (value < 0 or value > 255)
		{
			return false;
		}
		components.push_back(value);
	}
	return components.size() == 4;
}
