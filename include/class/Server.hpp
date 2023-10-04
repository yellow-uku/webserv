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
# define DEFAULT_MAX_BODY_SIZE 1000000 // 1M

# ifndef DEFAULT_ROOT
#  define DEFAULT_ROOT "./www"
# endif

# define DEFAULT_403_PAGE "error_pages/403.html"
# define DEFAULT_404_PAGE "error_pages/404.html"

class Server
{
public:
	typedef std::vector<listen_t>		listen_type;
	typedef std::map<int, std::string>	error_page_type;
	typedef std::vector<std::string>	server_name_type;

private:

	ssize_t								max_body_size;
	std::string							root;
	std::vector<listen_t>				listen;
	std::map<int, std::string>			error_pages;
	std::vector<std::string> 			server_names;

public:
	Server();
	~Server();

public:
	void pushListen(const std::string& host, const std::string& port);
	void setMaxBodySize(ssize_t l);
	void setRoot(const std::string& rt);
	void pushServerName(const std::string& server_name);
	void pushErrorPage(int error_code, const std::string& error_page);

public:
	ssize_t getMaxBodySize() const ;
	const std::string& getRoot() const ;
	const std::vector<listen_t>& getListens() const ;
	const std::map<int, std::string>& getErrorPages() const ;
	const std::vector<std::string>& getServerNames() const ;

public:
	void print_everything();

	template <typename T>
	void printVectors(const std::vector<T>& vec);

	bool operator==(const std::string& serverName) const ;
	bool operator==(const listen_t& lst) const ;

public:
	std::map<std::string, Location>		locations;
};

#endif // CLASS_SERVER_HPP