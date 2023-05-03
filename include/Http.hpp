#pragma once

#include <string>

#include "Request.hpp"
#include "Response.hpp"

class Http
{
	public:
		void readRequest(const std::string &request);
		bool sendResponse();

	private:
		Request _req;
};
