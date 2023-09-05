#include "TCPserver.hpp"

void	TCPserver::createSocketAndAddToSet()
{
	for (size_t i = 0; i < serverData.size(); ++i)
	{
		for (size_t j = 0; j < serverData[i].info.location["/barev/"].allow_methods.size(); ++j)
		{
			std::cout << serverData[i].info.location["/barev/"].allow_methods[j] << " ";
		}
		std::cout << "\n";
		for (size_t j = 0; j < serverData[i].info.index_files.size(); ++j)
		{
			std::cout << serverData[i].info.index_files[j] << " ";
		}
		std::cout << "\n";
	}
	
	// for(std::map<std::string, ServerInfo>::iterator it = server_data.begin(); it != server_data.end(); ++it)
	// {
	// 	std::cout << (it->second).root << "\n";
	// }
}
