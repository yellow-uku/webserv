#include "TCPserver.hpp"

struct ServerInfo init_serv_infos_struct(int port)
{
	struct ServerInfo data;
	data.port = port;
	data.root = "./";
	data.index_files.push_back("index.html");

	return (data);
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Config conf(DEFAULT_FILE_PATH);

	try
	{
		conf.parse();

		// conf.print();

		TCPserver socket(conf);

		socket.server_loop();

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}
// #include "webserv.hpp"

// // void loop();

// int main(int argc, char **argv)
// {
// 	(void)argc;
// 	(void)argv;


// 	try
// 	{
// 		Config conf = ( argc > 1 ? Config(argv[1]) : Config() );
// 		// conf.getLocationData("127.0.0.1", "8080", "server2", "/a").printEverything();
// 		// loop();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << "Error: " << e.what() << '\n';
// 	}
// }
