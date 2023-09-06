#ifndef SERVER_UTILS_HPP
# define SERVER_UTILS_HPP

class ResponseHeaders
{
	public:
		void build_headers();
		ResponseHeaders();
		~ResponseHeaders();

	public:
		std::string		http_versus;
		std::string		http_status;
		std::string		connection;
		std::string		content_type;
		std::string		backslash_r_n;
		std::string		headers;
};

struct ClientInfo
{
	std::string							allRequest;
	std::string 						requestBody;
	std::map<std::string,std::string> 	requestHeaders;
	std::string							method;
	std::string							url;
	std::string							uri;
	std::string							full_path;
	std::string 						response;
	std::string							reqstFirstline;
};

struct	ServerInfo
{
	int									port;
	bool								autoindex;
	size_t	 							max_body_size;
	std::vector<std::string>			server_names;
	std::string							root;
	std::vector<std::string>			allow_methods;
	std::vector<std::string>			index_files;
	std::map<int, std::string>			error_pages;
	std::string	 						redirect; 
	std::map<std::string, ServerInfo> 	location;
	std::string							cgi;
	std::string							uploadDir;
};

struct server_t {
	ServerInfo					info;
	Server::listen_type			listens;
	std::vector<std::string>	server_names;

	bool operator==(const socket_t& socket) const { return std::find(listens.begin(), listens.end(), socket) != listens.end(); }
};

#endif// SERVER_UTILS_HPP