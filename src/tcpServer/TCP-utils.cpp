#include "TCPserver.hpp"

void	TCPserver::createSocket(int port)
{
	const int enable = 1;
	struct sockaddr_in servAddr;

	int portNum = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (portNum < 0)
	{
		std::cerr << "Fail socket" << std::endl;
		exit(0);
	}
	
	bzero(&servAddr, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);

	if (bind(portNum, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
	{
		perror("Bind");
		exit(0);
	}
	if (listen(portNum, 500) < 0)
	{
		perror("Listen");
		exit(0);
	}

	if (setsockopt(portNum, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
	{	
		perror("Setsockopt");
		exit(0);
	}

	fcntl(portNum, F_SETFL, O_NONBLOCK);

	FD_SET(portNum, &set);
	FD_SET(portNum, &undo);

	sock.push_back(portNum);
}

void	TCPserver::createSocketAndAddToSet(std::vector<struct Server_info> &data)
{
	Server_info forLoc;
	forLoc.root = "../../locWeb";
	forLoc.error_pages[404] = "404.html";
	forLoc.index_files.push_back("index.html");
	FD_ZERO(&undo);
	FD_ZERO(&wr_undo);

	for(std::vector<struct Server_info>::iterator it = data.begin();it != data.end();it++)
	{
		forLoc.redirect = "/barev/";
		createSocket(it->port);
		server_data[it->port] = *it;
		server_data[it->port].error_pages[404] = "404.html";
		server_data[it->port].error_pages[403] = "403.html";
		server_data[it->port].error_pages[413] = "413.html";
		server_data[it->port].error_pages[405] = "405.html";
		server_data[it->port].root = std::string("www/");
		server_data[it->port].autoindex = false;
		server_data[it->port].max_body_size = 1024;
		server_data[it->port].location["barev"] = forLoc;
		server_data[it->port].allow_methods.push_back("GET");
		server_data[it->port].allow_methods.push_back("POST");
	}
}
