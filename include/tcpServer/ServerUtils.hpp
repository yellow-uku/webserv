#ifndef SERVER_UTILS_HPP
# define SERVER_UTILS_HPP

class ResponseHeaders
{
	public:
		void build_headers();
		ResponseHeaders();
		~ResponseHeaders();

	public:
		std::string		http_version;
		std::string		http_status;
		std::string		connection;
		std::string		content_type;
		std::string		backslash_r_n;
		std::string		headers;
};

struct ClientInfo
{
	std::string							url;
	std::string							query;
	std::string							method;
	std::string 						response;
	std::string							full_path;
	std::string							allRequest;
	std::string 						requestBody;
	std::string							httpVersion;
	std::string							reqstFirstline;
	std::map<std::string,std::string> 	requestHeaders;
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
	bool operator==(const std::string& name) const { return std::find(server_names.begin(), server_names.end(), name) != server_names.end(); }
};

#endif// SERVER_UTILS_HPP