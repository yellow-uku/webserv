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
# include <signal.h>
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

# define READ 0
# define WRITE 1

# define CGI_TIMEOUT 3
# define SOCKET_TIMEOUT 8

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
	void			selectError(std::vector<socket_t> &allFd, fd_set& main_read, fd_set& main_write);


private:
	int				receive(ClientInfo& client, int clnt, socket_t& socket);
	void			parseRequest(ClientInfo& client);
	void			parseChunked(ClientInfo& client, size_t max_body_size);
	void			setUrlAndMethod(ClientInfo& client);
	void			setResponseFile(ClientInfo& client, socket_t& listen);
	void			printHttpMessage(ClientInfo& client);
	void			buildResponse(std::string& fileName, ResponseHeaders &heading, ServerInfo servData, bool dir, ClientInfo& client);
	bool			findKeyValue(std::string& line, ClientInfo& client);
	bool			sendResponse(int clnt);
	std::string		setContentType(ClientInfo& client);
	std::string		readLine(std::string &str);

private:
	bool			checkMethod(ResponseHeaders &heading, ServerInfo &servData, ClientInfo& client);
	bool			checkBodySize(ResponseHeaders &heading, ServerInfo &servData, ClientInfo& client);
	bool			isRedirect(ResponseHeaders &headers, ServerInfo &info, ClientInfo& client);
	void			callCgi(ServerInfo& servData, ClientInfo& client, std::string& response);
	void			cgiChild(ServerInfo& servData, ClientInfo& client, int pipe_from_child[2], int pipe_to_child[2]);
	std::string		correctIndexFile(std::string &filename, ServerInfo &servData, ResponseHeaders& heading);
	char * const *	setEnv(std::map<std::string, std::string>& env, ServerInfo& servData, ClientInfo& client);

private:
	ServerInfo&		getLocationData(const socket_t& socket, const std::string& host, const std::string& route);
	info_iterator	findLocation(const std::vector<server_t>::iterator& info, std::string route);

private:
	bool			isDir(std::string &dirname);
	bool			checkFile(std::string &fileName, ResponseHeaders & heading);
	bool			checkDir(std::string &dirName, ResponseHeaders & heading);
	std::string		readFile(std::string filename);
	std::string		listDir(std::string &dirname);

private:
	int				uploadFile(std::string& filename, std::string& type, std::string& body, std::string& upload);
	int				parseBody(std::string& body, std::string& headers, std::string& upload);
	int				post(std::string& type, std::string &body, std::string& upload);
	void			parsePostRequest(ClientInfo& client, ResponseHeaders& headers, std::string& type);
	bool			postMultipart(std::string& requestBody, std::string& boundary, std::string& uplaod);
	std::string		getBoundary(std::string contentType);


public:
	TCPserver(const Config& conf);

	void			server_loop();

	~TCPserver();
};

#endif // TCP_SERVER_HPP
