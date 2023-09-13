#include "TCPserver.hpp"

void TCPserver::parseRequest(int client_socket)
{
	size_t start = 0;

	std::string key, value;

	std::string line;

	std::string request = clients[client_socket].allRequest;

	clients[client_socket].reqstFirstline = readLine(request, start);
	while (true)
	{
		line = readLine(request, start);
		if (!findKeyValue(line, client_socket))
			break ;
	}

	if (start < request.size() - 1)
		clients[client_socket].requestBody = clients[client_socket].allRequest.substr(start);
	else
		clients[client_socket].requestBody = "";

	setUrlAndMethod(client_socket);
}

std::string TCPserver::readLine(std::string &all, size_t &start)
{
	size_t len;

	len = all.find("\n", start);

	if (len == std::string::npos)
		return "";

	std::string retVal;

	len = len - start + 1;
	retVal =  all.substr(start, len);

	start += len;

	return retVal;
}

bool TCPserver::findKeyValue(std::string &line, size_t index)
{
	size_t len = line.find(":");

	if (len == std::string::npos)
		return false;

	std::string kval, vval;

	kval = line.substr(0, len);

	while (line[len] && std::isspace(line[++len]));

	vval = line.substr(len, line.find("\n"));

	if (vval[vval.size() - 1] == '\n')
		vval.erase(vval.size() - 1);

	clients[index].requestHeaders[kval] = vval;

	return true;
}

void TCPserver::setUrlAndMethod(int client_socket)
{
	std::stringstream	ss;
	std::string& url = clients[client_socket].url;

	ss << clients[client_socket].reqstFirstline << "\n";

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
}

size_t TCPserver::urlLength(std::string &str)
{
	size_t len = str.find('?');

	if (len == std::string::npos)
		return str.size();

	return (len);
}
