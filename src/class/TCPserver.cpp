# include "TCPserver.hpp"

TCPserver::TCPserver(const Config& conf)
{
	myenv = NULL;

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
		servinfo.info.uploadDir = servinfo.info.root + "/upload_dir";
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
			servinfo.info.location[it->first].root = (it->second).getValueOf("root");
			servinfo.info.location[it->first].autoindex = ((it->second).getValueOf("autoindex") == "on");
			servinfo.info.location[it->first].cgi = (it->second).getValueOf("cgi");
			servinfo.info.location[it->first].uploadDir = (it->second).getValueOf("upload_dir");
		}

		serverData.push_back(servinfo);
	}

	getSockets(conf);

	// createSocketAndAddToSet();
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
	int max_fd;
	int rc, ret = 0;

	fd_set read;
	fd_set write;
	fd_set main_read;
	fd_set main_write;

	struct sockaddr_in clntAddr;
	socklen_t clntAddrlen = sizeof(clntAddr);

	std::vector<socket_t> allFd;

	for(std::vector<socket_t>::iterator it = sockets.begin(); it != sockets.end(); ++it)
	{
		std::cout << it->fd << ", " << it->host << ":" << it->port << "\n";
		allFd.push_back(*it);
	}

	for(std::vector<socket_t>::iterator it = allFd.begin(); it != allFd.end(); it++)
	{
		if (it->fd > ret)
			ret = it->fd;
	}

	max_fd = ret;
	ret = 0;

	init_sets(main_read, main_write);

	while(true)
	{
		read = main_read;
		write = main_write;

		rc = select(max_fd + 1, &read, &write, NULL, NULL);

		std::cout << "select returned, " << rc << "\n";

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
						// exit (0); /// TODO: something 
					}

					if (clnt > max_fd)
						max_fd = clnt;

					allFd.push_back(socket_t(clnt, it->host, it->port));

					FD_SET(clnt, &main_read);

					fcntl(clnt, F_SETFL, O_NONBLOCK);
				}
				else
				{
					ret = recvfully(i);
					if (ret <= 0)
					{
						close (i);
						clients.erase(i);
						allFd.erase(std::find(allFd.begin(), allFd.end(), i));
						FD_CLR(i, &main_write);
						FD_CLR(i, &main_read);
						std::cout << "ret <= 0\n";
						break ;
					}
					std::cout << clients[i].allRequest << "\n";
					setResponseFile(i, *(std::find(allFd.begin(), allFd.end(), i)));
					FD_SET(i, &main_write);
					FD_CLR(i, &main_read);
				}
			}
			if (FD_ISSET(i, &write))
			{
				sendResponse(i);
				close (i);
				clients.erase(clients.find(i));
				allFd.erase(std::find(allFd.begin(), allFd.end(), i));
				FD_CLR(i, &main_write);
				FD_CLR(i, &main_read);
			}
		}
	}
}

void	TCPserver::setResponseFile(int client_socket, const socket_t& listen)
{
	ServerInfo			servData;
	std::string			fileName;
	std::string			response;
	ResponseHeaders		heading;

	ServerInfo& info = std::find(serverData.begin(), serverData.end(), listen)->info;

	if (isLocation(info, clients[client_socket].url))
	{
		servData = correctInfos(info, clients[client_socket].url);
	}
	else
		servData = info;

	if (servData.root[servData.root.size() - 1] != '/')
		servData.root += "/";

	heading.http_status = "200";

	if (isRedirect(heading,servData,client_socket))
		return ;

	fileName = servData.root + clients[client_socket].url;

	if (!checkMethod(fileName, heading, servData, client_socket) || checkBodySize(fileName, heading, servData, client_socket))
	{
		buildResponse(fileName, heading, 0, client_socket);
	}
	else if (isDir(fileName))
	{
		std::cout << fileName << " name\n";
		if (checkDir(fileName, heading, servData))
		{
			std::cout << "what?\n";
			buildResponse(fileName, heading, 0, client_socket);
			return;
		}
		if (!servData.autoindex)
		{
			if (thereIsNoIndexFile(servData))
			{
				fileName = "";
				std::cout << "no index\n";
			}
			else
			{
				fileName = correctIndexFile(fileName,servData);
				std::cout << "found index\n";
			}
		}
		else
		{
			buildResponse(fileName,heading, 1, client_socket);
			return ;
		}
	}
	checkFile(fileName, heading, servData);// body,methods 
	buildResponse(fileName, heading, 0, client_socket);
}

/*
  	HTTP/1.1 200 OK\r\n"
	"Connection: Close\r\n"
	"Content-Type: text/css\r\n"
	"\r\n"
*/

std::string	TCPserver::find_and_set_cont_type(int i)
{
	if (clients[i].url.find(".css") != std::string::npos)
		return (std::string("text/css"));
	else if (clients[i].url.find(".jpg") != std::string::npos)
		return (std::string("image/jpeg"));

	//typeri checky avelacnel
	return (std::string("text/html"));
}

bool	TCPserver::thereIsNoIndexFile(ServerInfo &servData)
{
	return servData.index_files.size() == 0;
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

	return servData.root + "/" + servData.error_pages[404];
}

bool	TCPserver::isLocation(ServerInfo &info, std::string &name)
{
	return info.location.count(name);
}

ServerInfo TCPserver::correctInfos(ServerInfo &info, std::string &name)
{
	std::cout << name << " name\n";
	return info.location[name];
}

bool TCPserver::isRedirect(ResponseHeaders &headData, ServerInfo &servData, int i)//poxel prototype
{
	if (servData.redirect[0] == '/')
		servData.redirect.erase(0, 1);

	if(!servData.redirect.size() || clients[i].url == servData.redirect)
		return false;

	(void) headData;

	clients[i].response = "HTTP/1.1 307 Temporary Redirect\r\n"
	"HTTP/1.1 307 Temporary Redirect\r\n"
	"Location: " + servData.redirect +"\r\n"
	"Content-Length: 0\r\n"
	"Connection: close\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n"
	"\r\n";

	return true;
}

void TCPserver::buildResponse(std::string &fileName, ResponseHeaders &heading, bool dir, int client_socket)
{
	std::string response;
	std::string headers;
	
	if (!dir)
	{
		heading.content_type = find_and_set_cont_type(client_socket);
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		response += readFile(fileName);
		clients[client_socket].full_path = fileName;
		clients[client_socket].response = response;
	}
	else
	{
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		response += listDir(fileName);
		clients[client_socket].response = response;
		clients[client_socket].full_path = fileName;
	}
}

bool TCPserver::checkMethod(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket)
{
	for (size_t i = 0; i < servData.allow_methods.size(); ++i)
	{
		if (servData.allow_methods[i] == clients[client_socket].method)
			return true;
	}

	// std::cout << servData.allow_methods[0] << " " << clients[client_socket].method << "\n";

	heading.http_status = "405";
	fileName = servData.root + "/" + servData.error_pages[405];
	return false;
}

bool TCPserver::checkBodySize(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket)
{	
	if (servData.max_body_size != 0 && clients[client_socket].requestBody.size() > servData.max_body_size)
	{
		heading.http_status = "413";
		fileName = servData.root + "/" + servData.error_pages[413];

		return true;
	}
	return false;
}

unsigned int TCPserver::urlLength(std::string &str)
{
	size_t len = str.find('?');

	if (len == std::string::npos)
		return str.size();

	return (len);
}

TCPserver::~TCPserver(){
	// std::cout << *(char *)nullptr;
}