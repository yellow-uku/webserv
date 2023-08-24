#include "Server.hpp"

Server::Server(): max_body_size(DEFAULT_MAX_BODY_SIZE), listen(), error_pages(), server_names(), locations()
{

}

void Server::pushListen(const std::string& host, const std::string& port)
{
	listen_t a = listen_t(host, port);

	if (std::find(listen.begin(), listen.end(), a) != listen.end())
		throw std::runtime_error("duplicate listen " + host + ":" + port);

	listen.push_back(listen_t(host, port));
}

void Server::setMaxBodySize(size_t	l) { max_body_size = l; }

void Server::pushServerName(const std::string& server_name)
{
	if (std::find(server_names.begin(), server_names.end(), server_name) == server_names.end())
		server_names.push_back(server_name);
}

void Server::pushErrorPage(int error_code, const std::string& error_page) { error_pages[error_code] = error_page; }

size_t Server::getMaxBodySize() const { return max_body_size; }

const std::vector<listen_t>& Server::getListens() const { return listen; }

const std::map<int, std::string>& Server::getErrorPages() const { return error_pages; }

const std::vector<std::string>& Server::getServerNames() const { return server_names; }

void Server::print_everything()
{
	std::cout << "Server names: ";
	printVectors(server_names);
	std::cout << "Listen: ";
	printVectors(listen);
	std::cout << "Error pages: \n";

	for (std::map<int, std::string>::iterator it = error_pages.begin(); it != error_pages.end(); ++it)
		std::cout << "  " << it->first << " " << it->second << " \n";

	std::cout << "\n";
	std::cout << "Max body size: " << max_body_size << "\n";
	std::cout << "Locations: \n--------\n";

	for (LocationMap::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		it->second.printEverything("  ");
		std::cout << "++++++++++++++\n\n";
	}
	std::cout << "--------\n";
}

bool Server::operator==(const std::string& serverName)
{
	return std::find(server_names.begin(), server_names.end(), serverName) != server_names.end();
}

bool Server::operator==(const listen_t& lst)
{
	return std::find(listen.begin(), listen.end(), lst) != listen.end();
}

template <typename T>
void Server::printVectors(const std::vector<T>& vec)
{
	for (size_t i = 0; i < vec.size(); ++i)
	{
		std::cout << vec[i] << " ";
	}
	std::cout << "\n";
}

Server::~Server() { }
