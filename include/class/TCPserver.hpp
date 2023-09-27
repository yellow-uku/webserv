#ifndef TCP_SERVER_HPP
# define TCP_SERVER_HPP

# include <map>
# include <bitset>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <iostream>
# include <algorithm>

# include <netdb.h>
# include <fcntl.h>
# include <stdio.h>
# include <limits.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <dirent.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/in.h>

# include "Config.hpp"
# include "ServerUtils.hpp"

# define MAX_BUF (1024UL * 1024UL * 1024UL * 15UL)

class TCPserver
{
public:
	typedef std::map<std::string, ServerInfo>::iterator info_iterator;

private:
	std::vector<server_t>				serverData;
	std::vector<socket_t>				sockets;
	std::map<int, ClientInfo>			clients;
	std::map<std::string, std::string>	reqstdata;

private:
	int				createSocket(const char *name, const char *port);
	void			initFdSets(fd_set& master, fd_set& wrmaster);
	void			getSockets(const Config& conf);

private:
	int				receive(int clnt);
	void			parseRequest(int client_socket);
	void			setUrlAndMethod(int client_socket);
	void			setResponseFile(int client_socket, socket_t& listen);
	void			buildResponse(std::string &fileName, ResponseHeaders &heading, ServerInfo servData, bool dir, int client_socket);
	bool			findKeyValue(std::string &, size_t );
	bool			sendResponse(int clnt);
	std::string		setContentType(int client_socket);
	std::string		readLine(std::string &str);

private:
	bool			checkMethod(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket);
	bool			checkBodySize(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket);
	bool			isRedirect(ResponseHeaders &headers, ServerInfo &info, int client_socket);
	size_t			urlLength(std::string &str);
	std::string		correctIndexFile(std::string &filename, ServerInfo &servData, ResponseHeaders& heading);
	std::string		callCgi(const ServerInfo& servData, int client_socket);
	char * const *	setEnv(std::map<std::string, std::string>& env, const ServerInfo& servData, ClientInfo& client);

private:
	ServerInfo&		getLocationData(const socket_t& socket, const std::string& host, const std::string& route);
	info_iterator	findLocation(const std::vector<server_t>::iterator& info, std::string route);

private:
	bool			isDir(std::string &dirname);
	bool			checkFile(std::string &fileName, ResponseHeaders & heading, ServerInfo &servData);
	bool			checkDir(std::string &dirName, ResponseHeaders & heading, ServerInfo &servData);
	std::string		readFile(std::string filename);
	std::string		listDir(std::string &dirname);

private:
	void			postImage();
	void			postMultipart(std::string requestBody, std::string boundary, ResponseHeaders &headers);
	void			parsePostRequest(ClientInfo& client, ResponseHeaders& headers);
	std::string		getBoundary(std::string contentType, ResponseHeaders& headers);

public:
	TCPserver(const Config& conf);

	void			server_loop();

	~TCPserver();
};

#endif // TCP_SERVER_HPP
