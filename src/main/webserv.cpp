// #include "webserv.hpp"

// void loop();

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
# include "TCPserver.hpp"

struct Server_info init_serv_infos_struct(int  port)
{
    struct Server_info data;
    data.port = port;
	data.root = std::string("./");
	data.index_files.push_back(std::string("index.html"));
    return (data);
}


int main(int argc, char **argv)
{
	std::vector<struct Server_info> data;
	(void) argv;
	if (argc < 2)
	{
		std::cerr << "Wrong argument count" << std::endl;
		return (1);
	}
	for(int i = 1; i < argc; i++)
	{
		data.push_back(init_serv_infos_struct(atoi(argv[i])));
	}
	TCPserver socket(data);
	while (1)
	{
		socket.acceptclnt();
//		//std::cout << "-------------------------------\n";
//		//std::cout << "------------END-------------------\n";
	}
	return 0;
}