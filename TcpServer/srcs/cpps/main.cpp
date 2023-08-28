# include "../headers/clases.hpp"

struct server_infos init_serv_infos_struct(int  port)
{
    struct server_infos data;
    data.port = port;
	data.root = std::string("./");
	data.index_files.push_back(std::string("index.html"));
    return (data);
}


int main(int argc, char **argv)
{
	std::vector<struct server_infos> data;
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