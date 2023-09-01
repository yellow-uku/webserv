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

# include "Server_utils.hpp"

// root, index, autoindex, error_page, cgi,
// #		client_max_body_size, allow_methods, upload_dir, location, return.

class TCPserver
{
	public:

		TCPserver(std::vector<struct Server_info>); //poxvelu a map
		~TCPserver();

		int				recvfully(int clnt);
		void			acceptclnt();
		void			sendResponse(int clnt);

	private:

		void			parseFirstLine(std::string);
		void			setMethodToEnv(std::string);
		void			createEnvForCGI(void);
		void			mapToCharDblPtr();
		void			parseRequest(std::string &, int i);
		void			setUrl_and_Method(int i);
		void			createSocketAndAddToSet(std::vector<struct Server_info> &);
		void			createSocket(int port);
		void			setResponseFile(int i);
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
		unsigned int	url_lenght(std::string &str); //<-
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

	public:
		char 								**myenv;
		std::vector<int>					sock;
		Response_headers					resp_head; //headers // <-
		std::map<int, Client_info>			clients;
		std::map<std::string, std::string>	preEnv; // need or not
		std::map<std::string, std::string>	reqstdata; // <-
		std::map<short, Server_info>		server_data;
		fd_set								set; // yndhanur
		fd_set								wrset; // yndhanur
		fd_set								undo;//yndhanur
		fd_set								wr_undo;//yndhanur
};

#endif // TCP_SERVER_HPP
