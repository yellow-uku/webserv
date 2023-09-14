#include "TCPserver.hpp"

void TCPserver::setResponseFile(int client_socket, const socket_t& socket)
{
	std::string			fileName;
	std::string			response;
	ResponseHeaders		heading;

	ServerInfo& servData = getLocationData(socket, clients[client_socket].requestHeaders["Host"], clients[client_socket].url);

	heading.http_status = "200";

	if (isRedirect(heading, servData, client_socket))
		return ;

	fileName = servData.root + clients[client_socket].url;

	if (!checkMethod(fileName, heading, servData, client_socket) || checkBodySize(fileName, heading, servData, client_socket))
	{
		buildResponse(fileName, heading, servData, 0, client_socket);
		return ;
	}

	if (clients[client_socket].url == "" || clients[client_socket].httpVersion != "HTTP/1.1"
		|| clients[client_socket].requestHeaders["Host"] == "")
	{
		heading.http_status = "400";
		fileName = servData.root + "/" + servData.error_pages[400];
		buildResponse(fileName, heading, servData, 0, client_socket);

		return ;
	}

	if (clients[client_socket].method == "GET")
	{
		if (isDir(fileName))
		{
			if (checkDir(fileName, heading, servData))
			{
				buildResponse(fileName, heading, servData, 0, client_socket);
				return ;
			}
			if (!servData.autoindex)
			{
				fileName = correctIndexFile(fileName, servData);
			}
			else
			{
				buildResponse(fileName, heading, servData, 1, client_socket);
				return ;
			}
		}
	}
	else if (clients[client_socket].method == "POST")
	{
		callCgi(servData, client_socket);
		// std::fstream file((servData.uploadDir + clients[client_socket].url).c_str(), std::fstream::out);

		// std::cout << (servData.uploadDir + clients[client_socket].url).c_str() << "\n";

		// if (file.fail())
		// {
		// 	heading.http_status = "404";
		// 	fileName = servData.root + "/" + servData.error_pages[404];
		// 	perror("Fstream");
		// }
		// else
		// 	file << clients[client_socket].requestBody.c_str();
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

	checkFile(fileName, heading, servData);
	buildResponse(fileName, heading, servData, 0, client_socket);
}

void TCPserver::buildResponse(std::string &fileName, ResponseHeaders &heading, const ServerInfo servData, bool dir, int client_socket)
{
	std::string response;
	std::string headers;

	if (!dir)
	{
		heading.content_type = find_and_set_cont_type(client_socket);
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		if (fileName.rfind('.') != std::string::npos && fileName.substr(fileName.rfind('.')) == ".py")
		{
			response += callCgi(servData, client_socket);
		}
		else
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
