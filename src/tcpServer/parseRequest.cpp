#include "TCPserver.hpp"

void TCPserver::parseRequest(int client_socket)
{
	size_t start = 0;

	std::string key, value;

	std::string line;

	std::string request = clients[client_socket].allRequest;

	// clients[client_socket].reqstFirstline = readLine(request, start);

	while (true)
	{
		line = readLine(request, start);
		if (!findKeyValue(line, client_socket))
			break ;
	}

	// if (start < request.size() - 1)
	// 	clients[client_socket].requestBody = clients[client_socket].allRequest.substr(start);
	// else
	// 	clients[client_socket].requestBody = "";

	// setUrlAndMethod(client_socket);
}

std::string TCPserver::readLine(std::string &all, size_t &start)
{
	size_t len;
	start = 0;

	len = all.find("\r\n", start);

	if (len == std::string::npos)
		return "";

	std::string retVal;

	retVal = all.substr(start, len);

	all.erase(start, len + 2);

	start += len;

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

	ss << clients[client_socket].reqstFirstline;

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

	if (clients[client_socket].method.empty()) clients[client_socket].method = "NONE";
}

size_t TCPserver::urlLength(std::string &str)
{
	size_t len = str.find('?');

	if (len == std::string::npos)
		return str.size();

	return (len);
}
