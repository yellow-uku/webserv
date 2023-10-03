#include "TCPserver.hpp"

int main(int argc, char **argv)
{
	Config conf(argc > 1 ? argv[1] : DEFAULT_FILE_PATH);

	try
	{
		conf.parse();
		// conf.print();

		TCPserver socket(conf);

		std::cout << hex_to_int("F") << "\n";

		socket.server_loop();

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
