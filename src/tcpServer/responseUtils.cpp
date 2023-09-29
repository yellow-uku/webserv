#include "TCPserver.hpp"

void TCPserver::setResponseFile(ClientInfo& client, socket_t& socket)
{
	bool				isIndex = false;
	std::string			fileName;
	std::string			response;
	ResponseHeaders		heading;

	ServerInfo& servData = getLocationData(socket, client.requestHeaders["Host"], client.url);

	servData.socket = socket;

	heading.http_status = "200";

	if (isRedirect(heading, servData, client))
		return ;

	fileName = servData.root + client.url;

	if (!checkMethod(heading, servData, client) || checkBodySize(heading, servData, client))
	{
		buildResponse(fileName, heading, servData, 0, client);
		return ;
	}

	if (client.url.empty() || client.httpVersion != "HTTP/1.1"
		|| client.requestHeaders["Host"].empty())
	{
		heading.http_status = "400";
		buildResponse(fileName, heading, servData, 0, client);

		return ;
	}

	if (client.method == "POST"
		&& client.requestHeaders["Content-Length"].empty()
		&& client.requestHeaders["Transfer-Encoding"].empty())
	{
		heading.http_status = "411";
		buildResponse(fileName, heading, servData, 0, client);

		return ;
	}

	if (client.method == "GET")
	{
		if (isDir(fileName))
		{
			if (checkDir(fileName, heading))
			{
				buildResponse(fileName, heading, servData, 0, client);
				return ;
			}
			if (!servData.autoindex)
			{
				fileName = correctIndexFile(fileName, servData, heading);
				isIndex = true;
			}
			else
			{
				buildResponse(fileName, heading, servData, 1, client);
				return ;
			}
		}
		if (!isIndex)
			checkFile(fileName, heading);
	}
	else if (client.method == "DELETE")
	{
		if (std::remove((servData.uploadDir + client.url).c_str()) != 0)
		{
			heading.http_status = "404";
			perror("Remove");
		}
	}

	buildResponse(fileName, heading, servData, 0, client);
}

void TCPserver::buildResponse(std::string &fileName, ResponseHeaders &heading, ServerInfo servData, bool dir, ClientInfo& client)
{
	std::string type;
	std::string response;

	if (client.method == "POST")
	{
		parsePostRequest(client, heading, type);
	}

	int status = my_stoi(heading.http_status);

	if (!dir)
	{
		heading.content_type = setContentType(client);
		heading.build_headers();
		response = heading.headers;

		if ((status >= 300 && status <= 600))
			response += readFile(servData.root + servData.error_pages[status]);
		else
		{
			if (fileName.rfind('.') != std::string::npos && fileName.substr(fileName.rfind('.')) == ".py")
				callCgi(servData, client, response);
			else if (client.method == "GET")
				response += readFile(fileName);
			else if (client.method == "POST")
			{
				if (type == "multipart/form-data")
				{
					if (postMultipart(client.requestBody, client.boundary, servData.uploadDir))
						response += readFile(servData.root + servData.error_pages[500]);
					else
						response += "<center> <h1> Files uploaded! </h1> </center>";
				}
				else if (type == "image/jpeg" || type == "image/png")
				{
					// postImage(client.requestBody);
				}
			}
		}

		client.fullPath = fileName;
		client.response = response;
	}
	else
	{
		heading.build_headers();
		response = heading.headers + listDir(fileName);
		client.response = response;
		client.fullPath = fileName;
	}
}
