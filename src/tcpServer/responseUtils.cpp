#include "TCPserver.hpp"

void TCPserver::setResponseFile(int client_socket, socket_t& socket)
{
	bool				dir = false;
	std::string			fileName;
	std::string			response;
	ResponseHeaders		heading;

	ServerInfo& servData = getLocationData(socket, clients[client_socket].requestHeaders["Host"], clients[client_socket].url);

	servData.socket = socket;

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
				fileName = correctIndexFile(fileName, servData, heading);
			}
			else
			{
				buildResponse(fileName, heading, servData, 1, client_socket);
				return ;
			}
		}
		checkFile(fileName, heading, servData);
	}
	else if (clients[client_socket].method == "DELETE")
	{
		if (std::remove((servData.uploadDir + clients[client_socket].url).c_str()) != 0)
		{
			heading.http_status = "404";
			perror("Remove");
		}
	}

	buildResponse(fileName, heading, servData, 0, client_socket);
}

void TCPserver::buildResponse(std::string &fileName, ResponseHeaders &heading, ServerInfo servData, bool dir, int client_socket)
{
	std::string response;

	if (clients[client_socket].method == "POST")
		parsePostRequest(clients[client_socket], heading);

	int status = my_stoi(heading.http_status);

	std::cout << "\n\n" << fileName << "\n\n";

	if (!dir)
	{
		heading.content_type = setContentType(client_socket);
		heading.build_headers();
		response = heading.headers;

		if ((status >= 300 && status <= 600))
			response += readFile(servData.root + servData.error_pages[status]);
		else
		{
			if (clients[client_socket].method == "GET")
			{
				if (fileName.rfind('.') != std::string::npos && fileName.substr(fileName.rfind('.')) == ".py")
					response += callCgi(servData, client_socket);
				else
					response += readFile(fileName);
			}
			else if (clients[client_socket].method == "POST")
			{
				std::string type = clients[client_socket].requestHeaders["Content-Type"];

				if (type == "multipart/form-data")
				{
					// postMultipart();
				}
				else if (type == "image/jpeg" || type == "image/png")
				{
					// postImage();
				}
			}
		}

		clients[client_socket].fullPath = fileName;
		clients[client_socket].response = response;
	}
	else
	{
		heading.build_headers();
		response = heading.headers + listDir(fileName);
		clients[client_socket].response = response;
		clients[client_socket].fullPath = fileName;
	}
}
