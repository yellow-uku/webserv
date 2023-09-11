#include "TCPserver.hpp"

bool TCPserver::checkMethod(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket)
{
	for (size_t i = 0; i < servData.allow_methods.size(); ++i)
	{
		if (servData.allow_methods[i] == clients[client_socket].method)
			return true;
	}

	heading.http_status = "405";
	fileName = servData.root + "/" + servData.error_pages[405];
	return false;
}

bool TCPserver::checkBodySize(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData, int client_socket)
{	
	if (servData.max_body_size != 0 && clients[client_socket].requestBody.size() > servData.max_body_size)
	{
		heading.http_status = "413";
		fileName = servData.root + "/" + servData.error_pages[413];

		return true;
	}
	return false;
}

bool TCPserver::isRedirect(ResponseHeaders &headData, ServerInfo &servData, int client_socket)//poxel prototype
{
	if(!servData.redirect.size() || clients[client_socket].url == servData.redirect)
		return false;

	(void) headData;

	clients[client_socket].response = "HTTP/1.1 307 Temporary Redirect\r\n"
	"Location: " + servData.redirect + "\r\n"
	"Content-Length: 0\r\n"
	"Connection: close\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n"
	"\r\n";

	return true;
}

bool TCPserver::isLocation(ServerInfo &info, std::string &name)
{
	return info.location.count(name);
}
