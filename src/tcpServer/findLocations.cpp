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
			break ;

		route.erase(pos);
	}

	return it->info.location.find("/");
}

ServerInfo& TCPserver::getLocationData(const socket_t& socket, const std::string& host, const std::string& route)
{
	ServerInfo& info = serverData[0].info; // default location

	size_t count = std::count(serverData.begin(), serverData.end(), socket);

	std::vector<server_t>::iterator it = std::find(serverData.begin(), serverData.end(), socket);

	std::string serverName = host.substr(0, host.find(':'));

	if (count == 1)
	{
		TCPserver::info_iterator l_it = findLocation(it, route);

		return l_it == it->info.location.end() ? it->info : l_it->second;
	}

	if (count > 1)
	{
		it = std::find(serverData.begin(), serverData.end(), serverName);

		if (it == serverData.end())
		{
			it = serverData.begin();

			for (size_t i = 0; i < serverData.size(); ++i)
			{
				if (std::find(serverData[i].server_names.begin(), serverData[i].server_names.end(), "") != serverData[i].server_names.end())
				{
					std::advance(it, i);
					break ;
				}
			}
		}

		TCPserver::info_iterator l_it = findLocation(it, route);

		return l_it == it->info.location.end() ? it->info : l_it->second;
	}

	return info;
}
