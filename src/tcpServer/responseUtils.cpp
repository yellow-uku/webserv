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
		checkFile(fileName, heading, servData);
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

	buildResponse(fileName, heading, servData, 0, client_socket);
}

void	parsePostRequest(std::string	requestBody, std::string boundary, ResponseHeaders &headers)
{
	size_t final = requestBody.find(boundary + "--");
	if (final == requestBody.npos)
	{
		headers.http_status = "400";
		return ;
	}

	size_t	start_index = 0;
	for (;;)
	{
		size_t start = requestBody.find(boundary , start_index) + boundary.size();

		if (requestBody.substr(start, 2) == "--")
			break ;

		size_t end = requestBody.find(boundary, start);

		std::string temp = requestBody.substr(start, end - start + 1);

		std::string a = temp.substr(temp.find("\r\n\r\n") + 4);

		FILE * fd = fopen("konichiwa.png", "wb");

		fwrite(a.c_str(), sizeof (a[0]), a.size(), fd);

		std::cout << "|" << a << "|" << std::endl;

		start_index = end;
	}
}

std::string	getBoundary(std::string contentType, ResponseHeaders& headers)
{
	size_t index = contentType.find("boundary=");

	if (index == contentType.npos)
	{
		headers.http_status = "400";
		return "";
	}

	return (contentType.substr(index + 9));
}

void TCPserver::buildResponse(std::string &fileName, ResponseHeaders &heading, const ServerInfo servData, bool dir, int client_socket)
{
	std::string response;
	std::string headers;

	// if method == post && content type == multipart
	std::string boundary = "--" + getBoundary(clients[client_socket].requestHeaders["Content-Type"], heading);

	parsePostRequest(clients[client_socket].requestBody, boundary, heading);

	int status = my_stoi(heading.http_status);

	if (!dir)
	{
		heading.content_type = setContentType(client_socket);
		heading.build_headers();
		headers = heading.headers;
		response = headers;

		if ((status >= 300 && status <= 600) || clients[client_socket].method == "DELETE")
			response += readFile(fileName);
		if (fileName.rfind('.') != std::string::npos && fileName.substr(fileName.rfind('.')) == ".py")
			response += callCgi(servData, client_socket);
		else if (clients[client_socket].method == "POST")
		{

		}
		else
			response += readFile(fileName);

		clients[client_socket].fullPath = fileName;
		clients[client_socket].response = response;
	}
	else
	{
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		response += listDir(fileName);
		clients[client_socket].response = response;
		clients[client_socket].fullPath = fileName;
	}
}
