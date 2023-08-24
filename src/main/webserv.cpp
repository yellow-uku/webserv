#include "webserv.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Config conf = ( argc > 1 ? Config(argv[1]) : Config() );

	try
	{
		conf.getLocationData("127.0.0.1", "8080", "server2", "/a").printEverything("");
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << '\n';
	}
}
