#ifdef LEAKS
#include <cstdlib>
#endif

#include "WebServ.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	try
	{
		if (argc > 2)
		{
			std::cerr << "Usage: " << argv[0] << " [config_file]" << std::endl;
			return 1;
		}
		if (argc == 2)
		{
			WebServ::startServers(argv[1]);
		}
		else
		{
			WebServ::startServers();
		}
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}

#ifdef LEAKS
	system("leaks a.out");
#endif

	return 0;
}
