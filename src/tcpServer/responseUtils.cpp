#include "TCPserver.hpp"

void TCPserver::setResponseFile(int client_socket, const socket_t& listen)
{
	ServerInfo			servData;
	std::string			fileName;
	std::string			response;
	ResponseHeaders		heading;

	ServerInfo& info = (std::find(serverData.begin(), serverData.end(), listen))->info;

	if (isLocation(info, clients[client_socket].url))
	{
		servData = correctInfos(info, clients[client_socket].url);
	}
	else
		servData = info;

	heading.http_status = "200";

	if (isRedirect(heading,servData,client_socket))
		return ;

	fileName = servData.root + clients[client_socket].url;

	if (!checkMethod(fileName, heading, servData, client_socket) || checkBodySize(fileName, heading, servData, client_socket))
	{
		buildResponse(fileName, heading, 0, client_socket);
		return ;
	}
	if (isDir(fileName))
	{
		if (checkDir(fileName, heading, servData))
		{
			buildResponse(fileName, heading, 0, client_socket);
			return;
		}
		if (!servData.autoindex)
		{
			if (thereIsNoIndexFile(servData))
			{
				fileName = "";
			}
			else
			{
				fileName = correctIndexFile(fileName,servData);
			}
		}
		else
		{
			buildResponse(fileName,heading, 1, client_socket);
			return ;
		}
	}
	checkFile(fileName, heading, servData);// body,methods 
	buildResponse(fileName, heading, 0, client_socket);
}

void TCPserver::buildResponse(std::string &fileName, ResponseHeaders &heading, bool dir, int client_socket)
{
	std::string response;
	std::string headers;
	
	if (!dir)
	{
		heading.content_type = find_and_set_cont_type(client_socket);
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		response += readFile(fileName);
		clients[client_socket].full_path = fileName;
		clients[client_socket].response = response;
	}
	else
	{
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		response += listDir(fileName);
		clients[client_socket].response = response;
		clients[client_socket].full_path = fileName;
	}
}
