#include "Server.hpp"

Server::Server(): max_body_size(-1), root(), listen(), error_pages(), server_names(), locations()
{

}

void Server::pushListen(const std::string& host, const std::string& port)
{
	listen_t a = listen_t(host, port);

	if (std::find(listen.begin(), listen.end(), a) != listen.end())
		throw std::runtime_error("duplicate listen " + host + ":" + port);

	listen.push_back(listen_t(host, port));
}

void Server::setMaxBodySize(ssize_t l) { max_body_size = l; }

void Server::setRoot(const std::string& rt)
{
	root = (rt[0] == '/' ? rt : DEFAULT_ROOT + rt);

	if (root[root.size() - 1] != '/')
		root += '/';
}

void Server::pushServerName(const std::string& server_name)
{
	if (std::find(server_names.begin(), server_names.end(), server_name) == server_names.end())
		server_names.push_back(server_name);
}

void Server::pushErrorPage(int error_code, const std::string& error_page) { error_pages[error_code] = error_page; }

ssize_t Server::getMaxBodySize() const { return max_body_size; }

const std::string& Server::getRoot() const { return root; }

const std::vector<listen_t>& Server::getListens() const { return listen; }

const std::map<int, std::string>& Server::getErrorPages() const { return error_pages; }

const std::vector<std::string>& Server::getServerNames() const { return server_names; }

void Server::print_everything()
{
	std::cout << "ROOT:\t\t\t" << root << "\n";
	std::cout << "SERVER_NAMES:\t";
	printVectors(server_names);
	std::cout << "LISTEN:\t\t";
	printVectors(listen);
	std::cout << "ERROR_PAGES: \n";

	int i = 0;
	for (std::map<int, std::string>::iterator it = error_pages.begin(); it != error_pages.end(); ++it)
	{
		std::cout << "\t" << ++i <<". " <<  it->first << " " << it->second << " \n";
	}

	std::cout << "\nMAX_BODY_SIZE:\t"<< max_body_size << "\n\n";
	std::cout << "\t\tLOCATIONS:\n\n";

	i = 0;
	for (LocationMap::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		std::cout << "--------------- LOCATION: " << it->first << " ---------------\n\n";
		it->second.printEverything();
	}
}

bool Server::operator==(const std::string& serverName) const
{
	return std::find(server_names.begin(), server_names.end(), serverName) != server_names.end();
}

bool Server::operator==(const listen_t& lst) const
{
	return std::find(listen.begin(), listen.end(), lst) != listen.end();
}

template <typename T>
void Server::printVectors(const std::vector<T>& vec)
{
	std::cout << "\t";
	for (size_t i = 0; i < vec.size(); ++i)
	{
		std::cout << vec[i] << " ";
	}
	std::cout << "\n";
}

Server::~Server() { }
