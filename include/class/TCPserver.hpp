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
# include <sys/stat.h>
# include <sys/types.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <netinet/in.h>

# include "Config.hpp"
# include "Server_utils.hpp"

# define MAX_BUF 4096

struct server_t {
	int 		port;
	std::string	host;
	Server_info info;
	std::vector<std::string> server_names;
};

// root, index, autoindex, error_page, cgi,
// #		client_max_body_size, allow_methods, upload_dir, location, return.

class TCPserver
{
	public:

		TCPserver(const Config& conf);
		TCPserver(std::vector<struct Server_info>); //poxvelu a map
		TCPserver(std::vector< Server_info> data, ConfigParser conf);

		~TCPserver();

		int				recvfully(int clnt);
		void			server_loop();
		void			sendResponse(int clnt);

	private:

		void			parseFirstLine(std::string);
		void			setMethodToEnv(std::string);
		void			createEnvForCGI(void);
		void			mapToCharDblPtr();
		void			parseRequest(std::string &, int i);
		void			setUrlAndMethod(int i);
		void			createSocketAndAddToSet();
		// void			createSocket(int port);
		int 			createSocket(const char *name, const char *port);
		void			setResponseFile(int i, const socket_t& listen);
		void			buildResponse(std::string &fileName,Response_headers & heading, bool dir, int i);
		bool			thereIsNoIndexFile(struct Server_info &servData);
		bool			isDir(std::string &);
		bool			isLocation(struct Server_info &,std::string &);
		bool 			isRedirect(Response_headers &, struct Server_info &, int);
		bool 			findKeyValue(std::string &, size_t );
		bool			checkFile(std::string &fileName,Response_headers & heading,struct Server_info &servData);
		bool			checkDir(std::string &dirName,Response_headers & heading,struct Server_info &servData);
		bool			checkMethod(std::string &fileName,Response_headers &heading, struct Server_info &servData,int i);
		bool			checkBodySize(std::string &fileName,Response_headers &heading, struct Server_info &servData,int i);
		unsigned int	urlLength(std::string &str); //<-
		std::string		readLine(std::string &,size_t &);
		std::string		readFile(std::string filename);
		std::string		findFile(std::string);
		std::string		getMethod();
		std::string		getUrl();
		std::string		gethttpvers();
		std::string		find_and_set_cont_type(int i);
		std::string		correctIndexFile(std::string &,struct Server_info &servData);
		std::string		listDir(std::string &);	
		Server_info		correctInfos(struct Server_info &,std::string &);

	private: // dve
		void			 getSockets(const Config& conf);
		void			 init_sets(fd_set& master, fd_set& wrmaster);

	public:
		char 								**myenv;
		std::vector<socket_t>				sockets;
		// Response_headers					resp_head; //headers // <-
		std::map<int, Client_info>			clients;
		std::map<std::string, std::string>	preEnv; // need or not
		std::map<std::string, std::string>	reqstdata; // <-
		std::map<std::string, Server_info>	server_data;
		std::vector<server_t>				server_vec;
};

#endif // TCP_SERVER_HPP
