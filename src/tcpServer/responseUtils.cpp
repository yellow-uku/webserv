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

	if (clients[client_socket].url == "" || clients[client_socket].httpVersion != "HTTP/1.1"
		|| clients[client_socket].requestHeaders["Host"] == "")
	{
		heading.http_status = "400";
		fileName = servData.root + "/" + servData.error_pages[400];
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
	}
	else if (clients[client_socket].method == "POST")
	{
		std::fstream file((servData.uploadDir + clients[client_socket].url).c_str(), std::fstream::out);

		std::cout << (servData.uploadDir + clients[client_socket].url).c_str() << "\n";

		if (file.fail())
		{
			heading.http_status = "404";
			fileName = servData.root + "/" + servData.error_pages[404];
			perror("Fstream");
		}
		else
			file << clients[client_socket].requestBody.c_str();
	}
	else if (clients[client_socket].method == "DELETE")
	{
		if (std::remove((servData.uploadDir + clients[client_socket].url).c_str()) != 0)
		{
			heading.http_status = "404";
			fileName = servData.uploadDir + servData.error_pages[404];
			perror("Remove");
		}
	}
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
