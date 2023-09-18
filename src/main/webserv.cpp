#include "TCPserver.hpp"

int main(int argc, char **argv, char **envp)
{
	Config conf(argc > 1 ? argv[1] : DEFAULT_FILE_PATH);

	// std::string abcd("abcd");

	// abcd.erase(0, 2);

	// std::cout << abcd << "\n";
	// std::cout << abcd.length() << "\n";

	try
	{
		conf.parse();
		conf.print();

		TCPserver socket(conf);

		socket.server_loop();

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
