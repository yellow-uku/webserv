#ifndef CLASS_SERVER_HPP
# define CLASS_SERVER_HPP

# include <map>
# include <vector>
# include <iostream>
# include <algorithm>

# include "Util.hpp"
# include "Location.hpp"

# define DEFAULT_HOST ""
# define DEFAULT_PORT "8080"
# define DEFAULT_SERVER_NAME ""
# define DEFAULT_MAX_BODY_SIZE 1000

# define DEFAULT_403_PAGE "error_pages/403.html"
# define DEFAULT_404_PAGE "error_pages/404.html"

class Server
{
private:
	size_t								max_body_size;
	std::vector<listen_t>				listen;
	std::map<int, std::string>			error_pages;
	std::vector<std::string> 			server_names;

public:
	Server();
	~Server();

public:
	void pushListen(const std::string& host, const std::string& port);
	void setMaxBodySize(size_t l);
	void pushServerName(const std::string& server_name);
	void pushErrorPage(int error_code, const std::string& error_page);

public:
	size_t getMaxBodySize() const ;
	const std::vector<listen_t>& getListens() const ;
	const std::map<int, std::string>& getErrorPages() const ;
	const std::vector<std::string>& getServerNames() const ;

public:
	void print_everything();

	template <typename T>
	void printVectors(const std::vector<T>& vec);

	bool operator==(const std::string& serverName);
	bool operator==(const listen_t& lst);

public:
	std::map<std::string, Location>		locations;
};

#endif // CLASS_SERVER_HPP