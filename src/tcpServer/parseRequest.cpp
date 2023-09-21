#include "TCPserver.hpp"

void TCPserver::parseRequest(int client_socket)
{
	std::string line, key, value;

	std::string request = clients[client_socket].allRequest;

	while (true)
	{
		line = readLine(request);
		if (!findKeyValue(line, client_socket))
			break ;
	}

	clients[client_socket].allRequest.erase(0, clients[client_socket].allRequest.find("\r\n\r\n") + 4);
}

std::string TCPserver::readLine(std::string &all)
{
	size_t len;

	len = all.find("\r\n");

	if (len == std::string::npos)
		return "";

	std::string retVal;

	retVal = all.substr(0, len);

	all.erase(0, len + 2);

	return retVal;
}

bool TCPserver::findKeyValue(std::string &line, size_t index)
{
	size_t len = line.find(":");

	if (len == std::string::npos)
		return false;

	std::string key, value;

	key = line.substr(0, len);

	while (line[len] && std::isspace(line[++len]));

	value = line.substr(len, line.find("\r\n"));

	clients[index].requestHeaders[key] = value;

	return true;
}

void TCPserver::setUrlAndMethod(int client_socket)
{
	std::stringstream ss;
	std::string& url = clients[client_socket].url;

	std::string firstLine = clients[client_socket].allRequest.substr(0, clients[client_socket].allRequest.find("\n"));
	clients[client_socket].allRequest.erase(0, clients[client_socket].allRequest.find("\n") + 1);

	ss << firstLine;

	ss >> clients[client_socket].method;
	ss >> clients[client_socket].url;
	ss >> clients[client_socket].httpVersion;

	if (url.find('?') != std::string::npos)
	{
		clients[client_socket].query = url.substr(url.find('?') + 1);
		url.erase(url.find('?'));
	}

	// erase last / of the url
	if (url != "/" && url[url.size() - 1] == '/')
		url.erase(url.size() - 1, 1);

	std::cout << "method->" << clients[client_socket].method << std::endl;
	std::cout << "url->" << clients[client_socket].url << std::endl;
	std::cout << "query->" << clients[client_socket].query << std::endl;
	std::cout << "HTTP version->" << clients[client_socket].httpVersion << std::endl << std::endl;

	if (clients[client_socket].method.empty())
		clients[client_socket].method = "NONE";
}

size_t TCPserver::urlLength(std::string &str)
{
	size_t len = str.find('?');

	if (len == std::string::npos)
		return str.size();

	return (len);
}
