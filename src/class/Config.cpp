#include "Config.hpp"

Config::Config(): ConfigParser(DEFAULT_FILE_PATH), defaultLocation()
{
	defaultLocation.pushIndexes("index.html");
	defaultLocation.pushMethods("GET");
	defaultLocation.pushMethods("POST");
}

Config::Config(const std::string& filename): ConfigParser(filename), defaultLocation()
{
	defaultLocation.pushIndexes("index.html");
	defaultLocation.pushMethods("GET");
	defaultLocation.pushMethods("POST");
}

const Config::std::vector<Server>& getServers() { return servers; }

const Location& Config::getLocationData(const std::string& host, const std::string& port, const std::string& serverName, const std::string& route)
{
	size_t count = std::count(servers.begin(), servers.end(), listen_t(host, port));

	ConfigParser::iterator it = std::find(servers.begin(), servers.end(), listen_t(host, port));

	if (count == 1)
	{
		ConfigParser::location_iterator l_it = it->locations.find(route);

		return l_it == it->locations.end() ? defaultLocation : l_it->second;
	}

	if (count > 1)
	{
		ConfigParser::iterator it = std::find(servers.begin(), servers.end(), serverName);

		if (it == servers.end())
			it = servers.begin();

		ConfigParser::location_iterator l_it = it->locations.find(route);

		return l_it == it->locations.end() ? defaultLocation : l_it->second;
	}

	if (count == 0)
	{
		ConfigParser::iterator it = std::find(servers.begin(), servers.end(), listen_t("", port));

		ConfigParser::location_iterator l_it = it->locations.find(route);

		return l_it == it->locations.end() ? defaultLocation : l_it->second;
	}

	return defaultLocation;
}

Config::~Config() { }
