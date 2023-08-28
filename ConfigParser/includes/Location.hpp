# ifndef	LOCATION_HPP
	# define	LOCATION_HPP

# include <vector>
# include <map>
# include <string>

# include	"Cgi.hpp"
# include	"Root.hpp"
# include	"Index.hpp"
# include	"Autoindex.hpp"
# include	"ErrorPage.hpp"
# include	"ClientMaxBodySize.hpp"
# include	"AllowMethods.hpp"
# include	"UploadDir.hpp"

class Location
{
	private:
		Root					root;
		Index					index;
		AutoIndex				autoIndex;
		std::vector<ErrorPage>	errorPages;
		ClientMaxBodySize		maxBodySize;
		AllowMethods			allowMethods;
		Cgi						cgi;
		UploadDir				uploadDir;

		std::string				locationName;
		std::vector<Location>	childLocations;


	public:
		Location(/* args */);
		~Location();
};


# endif//	LOCATION_HPP


