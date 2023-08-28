# ifndef NGINX_CONFIG_FILE_HPP
	# define NGINX_CONFIG_FILE_HPP

#include	"Server.hpp"
#include	<vector>

class NginxConfigFile
{
	private:
		std::vector<Server>	servers;

	public:
		NginxConfigFile(/* args */);
		~NginxConfigFile();
};

# endif// NGINX_CONFIG_FILE_HPP