#include "TCPserver.hpp"

// void	TCPserver::createSocket(int port)
// {
// 	const int enable = 1;
// 	struct sockaddr_in servAddr;

// 	int portNum = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

// 	if (portNum < 0)
// 	{
// 		std::cerr << "Fail socket" << std::endl;
// 		exit(0);
// 	}
	
// 	bzero(&servAddr, sizeof(servAddr));

// 	servAddr.sin_family = AF_INET;
// 	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	servAddr.sin_port = htons(port);

// 	if (bind(portNum, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
// 	{
// 		perror("Bind");
// 		exit(0);
// 	}
// 	if (listen(portNum, 500) < 0)
// 	{
// 		perror("Listen");
// 		exit(0);
// 	}

// 	if (setsockopt(portNum, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
// 	{	
// 		perror("Setsockopt");
// 		exit(0);
// 	}

// 	fcntl(portNum, F_SETFL, O_NONBLOCK);

// 	FD_SET(portNum, &read);
// 	FD_SET(portNum, &main_read);

// 	sockets.push_back(portNum);
// }

void	TCPserver::createSocketAndAddToSet()
{
	for (size_t i = 0; i < server_vec.size(); ++i)
	{
		for (size_t j = 0; j < server_vec[i].info.location["/barev/"].allow_methods.size(); ++j)
		{
			std::cout << server_vec[i].info.location["/barev/"].allow_methods[j] << " ";
		}
		std::cout << "\n";
		for (size_t j = 0; j < server_vec[i].info.index_files.size(); ++j)
		{
			std::cout << server_vec[i].info.index_files[j] << " ";
		}
		std::cout << "\n";
	}
	
	// for(std::map<std::string, Server_info>::iterator it = server_data.begin(); it != server_data.end(); ++it)
	// {
	// 	std::cout << (it->second).root << "\n";
	// }
}
