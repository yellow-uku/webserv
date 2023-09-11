#include "TCPserver.hpp"

TCPserver::info_iterator TCPserver::findLocation(const std::vector<server_t>::iterator& it, std::string route)
{
	size_t pos;
 
	while (1)
	{
		TCPserver::info_iterator location = it->info.location.find(route);

		if (location != it->info.location.end())
			return location;

		pos = route.rfind('/');

		if (pos == std::string::npos)
			return it->info.location.end();
		
		route.erase(pos);
	}
}

ServerInfo& TCPserver::getLocationData(const socket_t& socket, const std::string& host, const std::string& route)
{
	ServerInfo& info = serverData[0].info; // default location (std::find(serverData.begin(), serverData.end(), socket))->info;

	size_t count = std::count(serverData.begin(), serverData.end(), socket);

	std::vector<server_t>::iterator it = std::find(serverData.begin(), serverData.end(), socket);

	std::string serverName = host.substr(0, host.find(':'));

	if (count == 1)
	{
		TCPserver::info_iterator l_it = findLocation(it, route);

		return l_it == it->info.location.end() ? info : l_it->second;
	}

	if (count > 1)
	{
		it = std::find(serverData.begin(), serverData.end(), serverName);

		if (it == serverData.end())
			it = serverData.begin();

		TCPserver::info_iterator l_it = findLocation(it, route);

		return l_it == it->info.location.end() ? info : l_it->second;
	}

	if (count == 0)
	{
		it = std::find(serverData.begin(), serverData.end(), socket_t("", socket.port));

		TCPserver::info_iterator l_it = findLocation(it, route);

		return l_it == it->info.location.end() ? info : l_it->second;
	}

	return info;
}
