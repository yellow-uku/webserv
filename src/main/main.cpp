#include "TCPserver.hpp"

struct Server_info init_serv_infos_struct(int port)
{
	struct Server_info data;
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

	// std::vector<struct Server_info> data;

	// if (argc < 2)
	// {
	// 	std::cerr << "Wrong argument count" << std::endl;
	// 	return (1);
	// }

	// for(int i = 1; i < argc; i++)
	// {
	// 	data.push_back(init_serv_infos_struct(atoi(argv[i])));
	// }

	TCPserver socket(conf);

	socket.server_loop();

	// std::cout << socket.preEnv["123123"] << "\n";

	// for (std::map<std::string, std::string>::iterator i = socket.preEnv.begin(); i != socket.preEnv.end(); i++)
	// {
	// 	std::cout << i->first << "\n";
	// }
	
	// for (size_t i = 0; socket.myenv[i]; ++i)
	// {
	// 	std::cout << socket.myenv[i] << "\n";
	// }
	
	// socket.server_loop();

	return 0;
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
