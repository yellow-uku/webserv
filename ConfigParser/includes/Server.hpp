# ifndef SERVER_HPP
	# define SERVER_HPP

#include	"ServerName.hpp"
#include	"Listen.hpp"
#include	"Root.hpp"
#include	"Index.hpp"
#include	"Autoindex.hpp"
#include	"ErrorPage.hpp"
#include	"ClientMaxBodySize.hpp"
#include	"AllowMethods.hpp"
#include	"Location.hpp"

class Server
{
	private:
		ServerName				serverName;
		Listen					listen;
		Root					root;
		Index					index;
		AutoIndex				autoIndex;
		std::vector<ErrorPage>	errorPages;
		ClientMaxBodySize		maxBodySize;
		AllowMethods			allowMethods;
		std::vector<Location>	locations;


	public:
		Server(/* args */);
		~Server();
};

# endif// SERVER_HPP