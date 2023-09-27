#ifndef CLASS_NGINX_CONFIG_HPP

# define CLASS_NGINX_CONFIG_HPP

# include <map>
# include <cmath>
# include <stack>
# include <vector>
# include <fstream>
# include <sstream>
# include <iostream>
# include <algorithm>

# include <errno.h>
# include <string.h>

# include "Util.hpp"
# include "Server.hpp"

# define DEFAULT_FILE_PATH "conf.d/webserv.conf"

# define IS_OUTSIDE_LOCATION(token, prevToken) (strncmp(prevToken.c_str(), token.c_str(), prevToken.length()) != 0)

#define MAX_BODY_SIZE_G (3L)
#define MAX_BODY_SIZE_M (1024L * MAX_BODY_SIZE_G)
#define MAX_BODY_SIZE_K (1024L * 1024L * MAX_BODY_SIZE_G)

class ConfigParser

{
public:
	typedef	std::map<std::string, std::string>	Hosts;

	typedef std::vector<Server>::iterator		iterator;
	typedef LocationMap::iterator				location_iterator;
	typedef	Hosts::iterator						host_iterator;

private:
	std::string file_path;

public:
	Hosts hosts;
	std::vector<Server> servers;

private:
	static const int			error_page_numbers[];
	static char const * const	error_pages[];
	static char const * const	allowed_methods[];
	static char const * const	array_value_directives_location[];
	static char const * const	single_value_directives_location[];

private:
	void	getHosts();
	void	generateTokens(const std::string &file);
	void	parseLocations(std::vector<std::string> &tokens);
	void	check_braces(const std::vector<std::string> tokens);
	bool	containsSpecialChar(const std::string& token);
	void	getHostValues(const std::string& line, std::string& key, std::string &value) const ;
	size_t	getActualBodySize(const std::string& token);

private:
	int		isValidIP(const std::string& host) const ;
	bool	isValidPort(const std::string& _port) const ;
	void	validationOfListen(std::string token, std::string& host, std::string& port) const ;
	std::string	storeIP(const std::string& _host, int count) const ;

private:
	void	root(const std::vector<std::string>& tokens, size_t& server_index, size_t& i);

	void	listen(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i);
	void	errorPage(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i);
	void	serverName(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i);
	void	maxBodySize(const std::vector<std::string>& tokens, size_t& server_index, size_t& location_level, size_t& i);
	void	init_locations(Location& location, const size_t& server_index);

	void	setter(Location& location, const std::string& name, const std::string& val);
	void	setVectors(Location& current_location, const std::vector<std::string>& tokens, size_t& i);
	void	setProperties(Location& current_location, const std::vector<std::string>& tokens, size_t& i);
	void	storeLocation(std::vector<std::string>& tokens, std::vector<std::string>& current_location, size_t& location_level, const size_t& server_index, size_t& i);
	void	setDefaults();

public:
	ConfigParser(const std::string &file_path);

	void	print();
	void	parse();

	~ConfigParser();
};

#endif // CLASS_NGINX_CONFIG_HPP