#ifdef LEAKS
#include <cstdlib>
#endif

#include "WebServ.hpp"
#include <iostream>

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;
	try
	{
		// if (argc == 2 and std::string(argv[1]) == "-t")
		// {
		// 	ConfigChecker configChecker;

		// 	std::cout << "The configuration file syntax is OK" << std::endl;
		// }
		WebServ::startServers();
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
