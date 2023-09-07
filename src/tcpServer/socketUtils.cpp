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

				sockets.push_back(socket_t(createSocket((host == "" ? NULL : host.c_str()), port.c_str()), host, port));
			}
		}
	}
}

int TCPserver::createSocket(const char *name, const char *port)
{
	int yes = 1;
	struct addrinfo hints;
	struct addrinfo *list;

	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;

	int status = getaddrinfo(name, port, &hints, &list);

	if (status != 0 || list == NULL)
		throw std::runtime_error(std::string("Error getting addrinfo: ") + gai_strerror(status));

	int fd;
	int bind_errno = 0;

	struct addrinfo *temp = list;

	while (temp)
	{
		fd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

		if (fd < 0)
		{
			perror("socket");
			throw std::runtime_error("Socket error");
		}

		fcntl(fd, F_SETFL, O_NONBLOCK);
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

		if (bind(fd, temp->ai_addr, temp->ai_addrlen) == 0)
		{
			if (listen(fd, 500) < 0)
			{
				perror("listen");
				freeaddrinfo(list);
				throw std::runtime_error("Listen error");
			}

			break ;
		}

		bind_errno = errno;

		temp = temp->ai_next;
	}

	if (!temp)
	{
		freeaddrinfo(list);
		throw std::runtime_error(std::string("Bind error: ") + strerror(bind_errno));
	}

	freeaddrinfo(list);

	return fd;
}
