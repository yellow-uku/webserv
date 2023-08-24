#ifndef CLASS_CONFIG_HPP
# define CLASS_CONFIG_HPP

#include "ConfigParser.hpp"

class Config : public ConfigParser
{
private:
	Location defaultLocation;

public:
	Config();
	Config(const std::string& filename);

	const std::vector<Server>& getServers();

	const Location& getLocationData(const std::string& host, const std::string& port, const std::string& serverName, const std::string& route);

	~Config();
};

#endif // CLASS_CONFIG_HPP