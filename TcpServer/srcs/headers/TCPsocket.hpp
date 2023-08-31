# pragma once
# include "clases.hpp"

struct client_info{
	std::string		allRequest;
	std::string 	requestBody;
	std::map<std::string,std::string> 	requestHeaders;
	std::string		method;
	std::string		url;
	std::string		uri;
	std::string		full_path;
	std::string 	response;
	std::string		reqstFirstline;


};

struct	server_infos{
	int				port;
	std::string			server_name;
	std::string			root;
	std::vector<std::string>	allow_methods;//ogtagorcac chi
	std::vector<std::string>	index_files;
	std::map<short,std::string> error_pages;
	bool				autoindex;
	size_t 			max_body_size;
	//cgi
	//upload dir
	std::string	 redirect; 
	//client_max_body_size
	std::map<std::string,struct server_infos> location;
};

// root, index, autoindex, error_page, cgi,
// #		client_max_body_size, allow_methods, upload_dir, location, return.

class TCPserver{
	public:
        	TCPserver(std::vector<struct server_infos>); //poxvelu a map
		int	acceptclnt();
		void	sendResponse(int clnt);
		int recvfully(int clnt);
        	~TCPserver();
	private:
		std::string readFile(std::string filename);
		void		parseFirstLine(std::string);
		void		setMethodToEnv(std::string);
		std::string	findFile(std::string);
		void		createEnvForCGI(void);
		void		mapToCharDblPtr();
		void    	parseRequest(std::string &, int i);
		std::string	getMethod();
		std::string	getUrl();
		std::string	gethttpvers();
		void		setUrl_and_Method(int i);
		std::string		find_and_set_cont_type(int i);
		void		createSocketAndAddToSet(std::vector<struct server_infos> &);
		void		createSocket(int port);
		void		setResponseFile(int i);
		bool		thereIsNoIndexFile(struct server_infos &servData);
		std::string correctIndexFile(std::string &,struct server_infos &servData);
		bool		isDir(std::string &);
		std::string listDir(std::string &);	
		bool		isLocation(struct server_infos &,std::string &);
		struct server_infos correctInfos(struct server_infos &,std::string &);
		bool 		isRedirect(response_headers &, struct server_infos &, int);
		void	buildResponse(std::string &fileName,response_headers & heading, bool dir, int i);
		bool checkFile(std::string &fileName,response_headers & heading,struct server_infos &servData);
		bool checkDir(std::string &dirName,response_headers & heading,struct server_infos &servData);
		bool checkMethod(std::string &fileName,response_headers &heading, struct server_infos &servData,int i);
		bool checkBodySize(std::string &fileName,response_headers &heading, struct server_infos &servData,int i);

//		struct sockaddr_in servAddr[2];
//////////////
		std::string readLine(std::string &,size_t &);
		bool findKeyValue(std::string &, size_t );
////////////
		std::map<int,struct client_info> clients;
		std::vector<int> sock;
		char 		**myenv;
		std::map<std::string,std::string>	preEnv; // need or not
		std::map<std::string,std::string>	reqstdata; // <-
		unsigned int	url_lenght(std::string &str); //<-
		fd_set		set; // yndhanur
		fd_set		wrset; // yndhanur
		fd_set		undo;//yndhanur
		fd_set		wr_undo;//yndhanur
		int		max_fd;
		response_headers resp_head; //headers // <-
		std::map<short,struct server_infos> server_data;
};

