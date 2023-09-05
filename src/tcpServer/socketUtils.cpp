#include "TCPserver.hpp"

void TCPserver::getSockets(const Config& conf)
{
	const std::vector<Server> servers = conf.servers;

	for (size_t i = 0; i < servers.size(); ++i)
	{
		Server::listen_type listens = servers[i].getListens();

		for (size_t j = 0; j < listens.size(); ++j)
		{
			if (std::find(sockets.begin(), sockets.end(), listens[j]) == sockets.end())
			{
				std::string host = listens[j].host;
				std::string port = listens[j].port;

				sockets.push_back(socket_t(createSocket(host == "" ? NULL : host.c_str(), port.c_str()), host, port));
			}
		}
	}
}

int TCPserver::createSocket(const char *name, const char *port)
{
	int fd;
	int status;
	int yes = 1;
	struct addrinfo hints;
	struct addrinfo *list;

	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;

	status = getaddrinfo(name, port, &hints, &list);

	if (status != 0 || list == NULL)
		throw std::runtime_error(std::string("Error getting addrinfo: ") + gai_strerror(status));

	fd = socket(list->ai_family, list->ai_socktype, list->ai_protocol);

	if (fd < 0)
	{
		perror("socket");
		throw std::runtime_error("socket error");
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

	if (bind(fd, list->ai_addr, list->ai_addrlen))
	{
		perror("bind");
		throw std::runtime_error("bind error");
	}

	if (listen(fd, 500) < 0)
	{
		perror("listen");
		throw std::runtime_error("listen error");
	}

	freeaddrinfo(list);

	return fd;
}
