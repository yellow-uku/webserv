#include "TCPserver.hpp"

bool TCPserver::checkMethod(ResponseHeaders &heading, ServerInfo &servData, ClientInfo& client)
{
	for (size_t i = 0; i < servData.allow_methods.size(); ++i)
	{
		if (servData.allow_methods[i] == client.method)
			return true;
	}

	heading.http_status = "405";
	return false;
}

bool TCPserver::checkBodySize(ResponseHeaders &heading, ServerInfo &servData, ClientInfo& client)
{	
	if (servData.max_body_size != 0 && client.requestBody.size() > servData.max_body_size)
	{
		heading.http_status = "413";

		return true;
	}
	return false;
}

bool TCPserver::isRedirect(ResponseHeaders &headData, ServerInfo &servData, ClientInfo& client)//poxel prototype
{
	if(!servData.redirect.size() || client.url == servData.redirect)
		return false;

	(void) headData;

	client.response = "HTTP/1.1 307 Temporary Redirect\r\n"
	"Location: " + servData.redirect + "\r\n"
	"Content-Length: 0\r\n"
	"Connection: close\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n"
	"\r\n";

	return true;
}
