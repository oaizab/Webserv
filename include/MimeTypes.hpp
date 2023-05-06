#pragma once

#include <map>
#include <string>

class MimeTypes
{
	public:
		static std::string getMimeType(const std::string& extension);
	
	private:
		static std::map<std::string, std::string> data;

		static void init();
};
