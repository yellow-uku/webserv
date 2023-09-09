#include "TCPserver.hpp"

void TCPserver::setResponseFile(int client_socket, const socket_t& listen)
{
	ServerInfo			servData;
	std::string			fileName;
	std::string			response;
	ResponseHeaders		heading;

	ServerInfo& info = (std::find(serverData.begin(), serverData.end(), listen))->info;

	servData = (isLocation(info, clients[client_socket].url) ? correctInfos(info, clients[client_socket].url) : info);

	heading.http_status = "200";

	if (isRedirect(heading,servData,client_socket))
		return ;

	fileName = servData.root + clients[client_socket].url;

	if (!checkMethod(fileName, heading, servData, client_socket) || checkBodySize(fileName, heading, servData, client_socket))
	{
		buildResponse(fileName, heading, 0, client_socket);
		return ;
	}

	if (clients[client_socket].method == "GET")
	{
		if (isDir(fileName))
		{
			if (checkDir(fileName, heading, servData))
			{
				buildResponse(fileName, heading, 0, client_socket);
				return;
			}
			if (!servData.autoindex)
			{
				fileName = correctIndexFile(fileName, servData);
			}
			else
			{
				buildResponse(fileName, heading, 1, client_socket);
				return ;
			}
		}
		checkFile(fileName, heading, servData);// body,methods 
		buildResponse(fileName, heading, 0, client_socket);
	}
	else if (clients[client_socket].method == "POST")
	{
		std::fstream file((servData.uploadDir + clients[client_socket].url).c_str(), std::fstream::out);

		if (file.fail())
			perror("Fstream");
		else
			file << clients[client_socket].requestBody.c_str();
	}
	else if (clients[client_socket].method == "DELETE")
	{
		std::remove((servData.uploadDir + clients[client_socket].url).c_str());
	}
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
