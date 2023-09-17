#include "TCPserver.hpp"

int main(int argc, char **argv, char **env)
{
	(void)argc;
	(void)argv;

	Config conf(argc > 1 ? argv[1] : DEFAULT_FILE_PATH);

	try
	{
		conf.parse();

		conf.print();

		TCPserver socket(conf);
		socket.myEnv = env;
		socket.server_loop();

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
