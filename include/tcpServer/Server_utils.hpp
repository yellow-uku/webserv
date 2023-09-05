#ifndef SERVER_UTILS_HPP
# define SERVER_UTILS_HPP

class Response_headers
{
	public:
		void build_headers();
		Response_headers();
		~Response_headers();

	public:
		std::string		http_versus;
		std::string		http_status;
		std::string		connection;
		std::string		content_type;
		std::string		backslash_r_n;
		std::string		headers;
};

struct Client_info
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

struct	Server_info
{
	int									port;
	bool								autoindex;
	std::vector<std::string>			server_name;//ogtagorcac chi
	std::string							root;
	std::vector<std::string>			allow_methods;
	std::vector<std::string>			index_files;
	std::map<int, std::string>			error_pages;
	unsigned int	 					max_body_size;
	std::string	 						redirect; 
	std::map<std::string, Server_info> 	location;
	//cgi
	//upload dir
};

#endif// SERVER_UTILS_HPP