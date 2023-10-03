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
		servinfo.info.cgi = DEFAULT_CGI;
		servinfo.info.redirect = "";
		servinfo.info.uploadDir = serv.getRoot() + UPLOAD_DIRECTORY;
		servinfo.info.autoindex = false;
		servinfo.info.index_files.push_back(DEFAULT_INDEX);

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

void TCPserver::initFdSets(fd_set& main_read, fd_set& main_write)
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

	struct timeval timeout;
	struct sockaddr_in clntAddr;
	socklen_t clntAddrlen = sizeof(clntAddr);

	std::vector<socket_t> allFd;

	for(std::vector<socket_t>::iterator it = sockets.begin(); it != sockets.end(); ++it)
	{
		std::cout << it->fd << ", " << it->host << ":" << it->port << "\n";

		if (it->fd > max_fd)
			max_fd = it->fd;

		allFd.push_back(*it);
	}

	initFdSets(main_read, main_write);

	while(1)
	{
		read = main_read;
		write = main_write;
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		rc = select(max_fd + 1, &read, &write, NULL, &timeout);

		// std::cout << "select: " << rc << "\n";

		try
		{
			if (rc < 0)
			{
				perror("Select error");
				selectError(allFd, main_read, main_write);
				continue ;
			}

			for (size_t i = 0; i < allFd.size(); ++i)
			{
				std::cout << "checking socket: " << allFd[i].fd << "\n";
				if (FD_ISSET(allFd[i].fd, &main_read) && std::find(sockets.begin(), sockets.end(), allFd[i].fd) == sockets.end())
				{
					if (time(NULL) - allFd[i].timeout >= SOCKET_TIMEOUT)
					{
						std::cout << "Socket " << allFd[i].fd << " timed out, closing.\n";

						close(allFd[i].fd);
						clients.erase(allFd[i].fd);

						FD_CLR(allFd[i].fd, &main_read);

						rc = -2; // skip the loop below
						allFd[i].fd = -1;
					}
				}
			}

			allFd.erase(std::remove(allFd.begin(), allFd.end(), -1), allFd.end());

			for (int i = 3; rc != -2 && i <= max_fd; ++i)
			{
				if (FD_ISSET(i, &read))
				{
					std::vector<socket_t>::iterator it = std::find(sockets.begin(), sockets.end(), i);

					if (it != sockets.end())
					{
						int clnt = accept(i, (struct sockaddr *)&clntAddr, &clntAddrlen);

						if (clnt < 0)
						{
							perror("Accept error");
							break ; // is this enough
						}

						if (clnt > max_fd)
							max_fd = clnt;

						allFd.push_back(socket_t(clnt, it->host, it->port));

						std::cout << "socket " << clnt << " added to set\n";
						FD_SET(clnt, &main_read);

						fcntl(clnt, F_SETFL, O_NONBLOCK);
					}
					else
					{
						std::cout << "socket " << i << " tries to receive\n";
						int res = receive(clients[i], i, *(std::find(allFd.begin(), allFd.end(), i)));

						if (res == 1)
						{
							std::cout << "socket " << i << " finished reading\n";
							setResponseFile(clients[i], *(std::find(allFd.begin(), allFd.end(), i)));

							FD_CLR(i, &main_read);
							FD_SET(i, &main_write);
						}
						else if (res < 0)
						{
							close (i);
							clients.erase(i);

							allFd.erase(std::find(allFd.begin(), allFd.end(), i));

							FD_CLR(i, &main_read);
							FD_CLR(i, &main_write);

							break ;
						}
					}
				}
				if (FD_ISSET(i, &write))
				{
					if (sendResponse(i))
					{
						std::cout << "socket " << i << " closed with write\n";
						close (i);
						clients.erase(clients.find(i));
						allFd.erase(std::find(allFd.begin(), allFd.end(), i));
						FD_CLR(i, &main_read);
						FD_CLR(i, &main_write);
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

void TCPserver::selectError(std::vector<socket_t> &allFd, fd_set& main_read, fd_set& main_write)
{
	for (size_t i = 0; i < allFd.size(); ++i)
	{
		if (std::find(sockets.begin(), sockets.end(), allFd[i].fd) == sockets.end())
		{
			close(allFd[i].fd);
			clients.erase(allFd[i].fd);
			allFd.erase(std::find(allFd.begin(), allFd.end(), allFd[i].fd));
		}
	}

	FD_ZERO(&main_read);
	FD_ZERO(&main_write);

	for (size_t i = 0; i < sockets.size(); ++i)
		FD_SET(sockets[i].fd, &main_read);
}

TCPserver::~TCPserver() { }
