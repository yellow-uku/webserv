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
# include "Server_utils.hpp"

# define MAX_BUF 4096

class TCPserver
{
public:
	typedef std::map<std::string, ServerInfo>::iterator info_iterator;

public:
	TCPserver(const Config& conf);

	int				recvfully(int clnt);
	void			server_loop();
	void			sendResponse(int clnt);

	~TCPserver();

private:
	int 			createSocket(const char *name, const char *port);
	void			setMethodToEnv(std::string);
	void			createEnvForCGI();
	void			mapToCharDblPtr();
	void			parseRequest(int client_socket);
	void			setUrlAndMethod(int client_socket);
	void			createSocketAndAddToSet();
	void			setResponseFile(int client_socket, const socket_t& listen);
	void			buildResponse(std::string &fileName, ResponseHeaders &heading, const ServerInfo servData, bool dir, int client_socket);
	bool			thereIsNoIndexFile(ServerInfo &servData);
	bool			isDir(std::string &dirname);
	bool			isLocation(ServerInfo &info, std::string &name);
	bool 			isRedirect(ResponseHeaders &headers, ServerInfo &info, int client_socket);
	bool 			findKeyValue(std::string &, size_t );
	bool			checkFile(std::string &fileName, ResponseHeaders & heading, ServerInfo &servData);
	bool			checkDir(std::string &dirName, ResponseHeaders & heading, ServerInfo &servData);
	bool			checkMethod(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket);
	bool			checkBodySize(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket);
	size_t			urlLength(std::string &str); //<-
	std::string		readLine(std::string &str, size_t &start);
	std::string		readFile(std::string filename);
	std::string		findFile(std::string filename);
	std::string		getMethod();
	std::string		getUrl();
	std::string		gethttpvers();
	std::string		find_and_set_cont_type(int client_socket);
	std::string		correctIndexFile(std::string &filename, ServerInfo &servData);
	std::string		listDir(std::string &dirname);	
	ServerInfo&		getLocationData(const socket_t& socket, const std::string& host, const std::string& route);
	info_iterator	findLocation(const std::vector<server_t>::iterator& info, std::string route);
	// ServerInfo		correctInfos(ServerInfo &info, std::string &name);

	std::string callCgi(const ServerInfo& servData, int client_socket);



private: // dve
	void			 getSockets(const Config& conf);
	void			 init_sets(fd_set& master, fd_set& wrmaster);

public:
	char 								**myenv;
	std::vector<socket_t>				sockets;
	// ResponseHeaders					resp_head; //headers // <-
	std::map<int, ClientInfo>			clients;
	std::map<std::string, std::string>	preEnv; // need or not
	std::map<std::string, std::string>	reqstdata; // <-
	std::vector<server_t>				serverData;
};

#endif // TCP_SERVER_HPP
