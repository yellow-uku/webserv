#include "TCPserver.hpp"

void TCPserver::parseRequest(int client_socket)
{
	size_t start = 0;

	std::string key, value;

	std::string line;

	std::string request = clients[client_socket].allRequest;

	clients[client_socket].reqstFirstline = readLine(request, start);

	while(true)
	{
		line = readLine(request, start);
		if(!findKeyValue(line, client_socket))
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
		return all;

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

	kval = line.substr(0,len);

	while (std::isspace(line[++len]));

	vval = line.substr(len, line.find("\n"));

	clients[index].requestHeaders[kval] = vval;

	return true;
}

void TCPserver::setUrlAndMethod(int client_socket)
{
	std::stringstream	ss;
	std::string 		url;
	std::string 		uri;
	std::string 		method;

	ss << clients[client_socket].reqstFirstline << "\n";

	std::getline(ss, method, ' ');

	if(clients[client_socket].reqstFirstline.find('?') != std::string::npos)
	{
		std::getline(ss, url, '?');
		std::getline(ss, uri, ' ');
	}
	else
	{
		std::getline(ss, url, ' ');
		uri = " ";
	}

	// if(url[0] == '/')
	// 	url.erase(0, 1); why need this ?

	clients[client_socket].method = method;
	clients[client_socket].url = url;
	clients[client_socket].uri = uri;

	std::cout << "method->" << method << std::endl;
	std::cout << "url->" << url << std::endl;
	std::cout << "uri->" << uri << std::endl;
	
}
