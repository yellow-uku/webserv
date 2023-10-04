#include "TCPserver.hpp"

void TCPserver::setResponseFile(ClientInfo& client, socket_t& socket)
{
	bool				isIndex = false;
	std::string			fileName;
	std::string			response;
	ResponseHeaders		heading;

	ServerInfo& servData = getLocationData(socket, client.requestHeaders["Host"], client.url);

	servData.socket = socket;

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
		&& ((client.requestHeaders["Content-Length"].empty()
		&& client.requestHeaders["Transfer-Encoding"].empty())
		|| client.chunkedFail || my_stos_t(client.requestHeaders["Content-Length"]) < 0))
	{
		heading.http_status = "411";
		buildResponse(fileName, heading, servData, 0, client);

		return ;
	}

	if (client.method == "GET")
	{
		if (isDir(fileName))
		{
			std::string index = correctIndexFile(fileName, servData, heading);

			if (checkDir(fileName, heading))
			{
				buildResponse(fileName, heading, servData, 0, client);
				return ;
			}
			if (index != "")
			{
				isIndex = true;
				fileName = index;
			}
			else if (servData.autoindex)
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
			heading.http_status = "403";
			perror("Remove");
		}
	}

	buildResponse(fileName, heading, servData, 0, client);
}

void TCPserver::buildResponse(std::string &fileName, ResponseHeaders &heading, ServerInfo servData, bool dir, ClientInfo& client)
{
	std::string type;
	std::string &response = client.response;

	if (client.method == "POST")
		parsePostRequest(client, heading, type);

	int status = my_stoi(heading.http_status);

	if (!dir)
	{
		heading.content_type = setContentType(client);
		heading.build_headers();

		if ((status >= 300 && status <= 600))
			response = readFile(servData.root + servData.error_pages[status]);
		else
		{
			if (client.method == "DELETE")
			{
				response = "<center><h1>Deleted file " + servData.root + client.url + "</h1></center>";
			}
			else if (fileName.rfind('.') != std::string::npos && fileName.substr(fileName.rfind('.')) == ".py")
			{
				callCgi(servData, client, response);
			}
			else if (client.method == "GET")
			{
				response = readFile(fileName);
			}
			else if (client.method == "POST")
			{
				if (type == "multipart/form-data")
				{
					if (postMultipart(client.requestBody, client.boundary, servData.uploadDir))
						response = readFile(servData.root + servData.error_pages[500]);
					else
						response = "<center> <h1> Files uploaded! </h1> </center>";
				}
				else if (type == "image/jpeg" || type == "image/png" || type == "text/plain")
				{
					if (post(type, client.requestBody, servData.uploadDir))
						response = readFile(servData.root + servData.error_pages[500]);
					else
						response = "<center> <h1> Files uploaded! </h1> </center>";
				}
			}
		}

		response = heading.headers + response;
	}
	else
	{
		heading.build_headers();
		response = heading.headers + listDir(fileName);
	}

	// std::cout << "RESPONSE:\n" << response << "\n";
}

std::string TCPserver::correctIndexFile(std::string &fileName, ServerInfo &servData, ResponseHeaders& headers)
{
	std::string full_path;

	for (std::vector<std::string>::iterator it = servData.index_files.begin(); it < servData.index_files.end(); ++it)
	{
		full_path = fileName + *it;
		if (access(full_path.c_str(), F_OK & R_OK) == 0)
			return full_path;
	}

	headers.http_status = "403";
	return "";
}

std::string	TCPserver::setContentType(ClientInfo& client)
{
	if (client.url.find(".css") != std::string::npos)
		return ("text/css");
	else if (client.url.find(".jpg") != std::string::npos
			|| client.url.find(".jpeg") != std::string::npos)
		return ("image/jpeg");
	else if (client.url.find(".png") != std::string::npos)
		return ("image/png");

	return ("text/html");
}
