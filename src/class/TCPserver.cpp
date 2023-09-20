#include "TCPserver.hpp"

TCPserver::TCPserver(const Config& conf)
{
	for(size_t i = 0; i < conf.servers.size(); ++i)
	{
		const Server& serv = conf.servers[i];

		server_t servinfo;

		servinfo.listens = serv.getListens();

		servinfo.server_names = serv.getServerNames();

		servinfo.info.error_pages = serv.getErrorPages();
		servinfo.info.max_body_size = serv.getMaxBodySize();
		servinfo.info.root = serv.getRoot();
		servinfo.info.cgi = "/bin/ls"; // ??
		servinfo.info.redirect = "";
		servinfo.info.uploadDir = serv.getRoot() + "/upload/";
		servinfo.info.autoindex = false;
		servinfo.info.index_files.push_back("index.html");

		servinfo.info.allow_methods.push_back("GET");
		servinfo.info.allow_methods.push_back("POST");
		servinfo.info.allow_methods.push_back("DELETE");

		for (std::map<std::string, Location>::const_iterator it = serv.locations.begin(); it != serv.locations.end(); ++it)
		{
			servinfo.info.location[it->first].error_pages = serv.getErrorPages();
			servinfo.info.location[it->first].max_body_size = serv.getMaxBodySize();

			servinfo.info.location[it->first].allow_methods = (it->second).getArrayOf("allow_methods");
			servinfo.info.location[it->first].index_files = (it->second).getArrayOf("index");

			servinfo.info.location[it->first].redirect = (it->second).getValueOf("return");
			servinfo.info.location[it->first].root = (it->second).getValueOf("root");
			servinfo.info.location[it->first].autoindex = ((it->second).getValueOf("autoindex") == "on");
			servinfo.info.location[it->first].cgi = (it->second).getValueOf("cgi");
			servinfo.info.location[it->first].uploadDir = (it->second).getValueOf("upload_dir");
		}

		serverData.push_back(servinfo);
	}

	getSockets(conf);
}

void TCPserver::init_sets(fd_set& main_read, fd_set& main_write)
{
	FD_ZERO(&main_read);
	FD_ZERO(&main_write);

	for (size_t i = 0; i < sockets.size(); ++i)
	{
		FD_SET(sockets[i].fd, &main_read);
	}
}

void TCPserver::server_loop()
{
	int rc;
	int max_fd = INT_MIN;

	fd_set read;
	fd_set write;
	fd_set main_read;
	fd_set main_write;

	struct sockaddr_in clntAddr;
	socklen_t clntAddrlen = sizeof(clntAddr);

	std::vector<int> acceptedFd;
	std::vector<socket_t> allFd;

	for(std::vector<socket_t>::iterator it = sockets.begin(); it != sockets.end(); ++it)
	{
		std::cout << it->fd << ", " << it->host << ":" << it->port << "\n";

		if (it->fd > max_fd)
			max_fd = it->fd;

		allFd.push_back(*it);
	}

	init_sets(main_read, main_write);

	while(1)
	{
		read = main_read;
		write = main_write;

		rc = select(max_fd + 1, &read, &write, NULL, NULL);

		try
		{
			if (rc == 0)
				continue ;

			for (int i = 3; i <= max_fd; ++i)
			{
				if (FD_ISSET(i, &read))
				{
					std::vector<socket_t>::iterator it = std::find(sockets.begin(), sockets.end(), i);

					if (it != sockets.end())
					{
						int clnt = accept(i, (struct sockaddr *)&clntAddr, &clntAddrlen);

						if (clnt < 0)
						{
							std::cerr << "accept failed -> " << errno << std::endl;
							perror("accept");
							break ; // is this enough
						}

						if (clnt > max_fd)
							max_fd = clnt;

						allFd.push_back(socket_t(clnt, it->host, it->port));

						FD_SET(clnt, &main_read);

						fcntl(clnt, F_SETFL, O_NONBLOCK);
					}
					else
					{
						int res = recvfully(i);

						if (res == 1)
						{
							setResponseFile(i, *(std::find(allFd.begin(), allFd.end(), i)));

							FD_SET(i, &main_write);
							FD_CLR(i, &main_read);
						}
						else if (res <= 0)
						{
							close (i);
							clients.erase(i);

							allFd.erase(std::find(allFd.begin(), allFd.end(), i));

							FD_CLR(i, &main_write);
							FD_CLR(i, &main_read);

							break ;
						}
					}
				}
				if (FD_ISSET(i, &write))
				{
					if (sendResponse(i))
					{
						close (i);
						clients.erase(clients.find(i));
						allFd.erase(std::find(allFd.begin(), allFd.end(), i));
						FD_CLR(i, &main_write);
						FD_CLR(i, &main_read);
					}
				}
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << '\n';
		}
	}
}

std::string	TCPserver::find_and_set_cont_type(int client_socket)
{
	if (clients[client_socket].url.find(".css") != std::string::npos)
		return ("text/css");
	else if (clients[client_socket].url.find(".jpg") != std::string::npos
			|| clients[client_socket].url.find(".jpeg") != std::string::npos)
		return ("image/jpeg");
	else if (clients[client_socket].url.find(".png") != std::string::npos)
		return ("image/png");

	//typeri checky avelacnel
	return ("text/html");
}

std::string TCPserver::correctIndexFile(std::string &fileName, ServerInfo &servData)
{
	std::string full_path;

	for (std::vector<std::string>::iterator it = servData.index_files.begin(); it < servData.index_files.end(); ++it)
	{
		full_path = fileName + *it;
		if (access(full_path.c_str(), F_OK & R_OK) == 0)
			return full_path;
	}

	return servData.root + "/" + servData.error_pages[403];
}

TCPserver::~TCPserver() { }
