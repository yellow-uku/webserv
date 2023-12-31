#include "TCPserver.hpp"

void TCPserver::parseRequest(ClientInfo& client)
{
	std::string line, key, value;

	std::string request = client.allRequest;

	while (true)
	{
		line = readLine(request);
		if (!findKeyValue(line, client))
			break ;
	}

	client.allRequest.erase(0, client.allRequest.find("\r\n\r\n") + 4);
}

std::string TCPserver::readLine(std::string &all)
{
	size_t len;

	len = all.find("\r\n");

	if (len == std::string::npos)
		return "";

	std::string retVal;

	retVal = all.substr(0, len);

	all.erase(0, len + 2);

	return retVal;
}

bool TCPserver::findKeyValue(std::string &line, ClientInfo& client)
{
	size_t len = line.find(":");

	if (len == std::string::npos)
		return false;

	std::string key, value;

	key = line.substr(0, len);

	while (line[len] && std::isspace(line[++len]));

	value = line.substr(len, line.find("\r\n"));

	client.requestHeaders[key] = value;

	return true;
}

void TCPserver::printHttpMessage(ClientInfo& client)
{
	std::vector<std::string> messageInfo;
	std::string line;
	std::istringstream	res(client.allRequest);

	while (std::getline(res, line))
		messageInfo.push_back(line);

	std::map<std::string,std::string>	messageHeaders;

    for (size_t i = 0; i < messageInfo.size(); ++i) {
        size_t colonPos = messageInfo[i].find(":");
        if (colonPos != std::string::npos) {
            std::string messageHeader = messageInfo[i].substr(0, colonPos);
			std::string messageValue = messageInfo[i].substr(colonPos + 2);
            messageHeaders[messageHeader] = messageValue;
        }
    }
	// for (std::map<std::string, std::string>::iterator it = messageHeaders.begin(); it != messageHeaders.end(); ++it)
	// 	std::cout << it->first << ": " << it->second << "\n";
	if (client.method == "GET")
	{
		std::cout << "\n";
		std::cout << client.method << " " << client.url << " " << client.httpVersion << "\n";
		std::cout << "Host: " << messageHeaders["Host"] << "\n";
		std::cout << "Accept: " << messageHeaders["Accept"] << "\n";
		std::cout << "\n";
	}
	else if (client.method == "POST")
	{
		std::cout << "\n";
		std::cout << client.method << " " << client.url << " " << client.httpVersion << "\n";
		std::cout << "User-Agent: " << messageHeaders["User-Agent"] << "\n";
		std::cout << "Host: " << messageHeaders["Referer"] << "\n";
		std::cout << "Accept-Language: " << messageHeaders["Accept-Language"] << "\n";
		std::cout << "Accept-Encoding: " << messageHeaders["Accept-Encoding"] << "\n";
		std::cout << "Connection: " << messageHeaders["Connection"] << "\n";
		std::cout << "\n";
	}
}

void TCPserver::setUrlAndMethod(ClientInfo& client)
{
	std::stringstream ss;
	std::string& url = client.url;

	std::string firstLine = client.allRequest.substr(0, client.allRequest.find("\n"));
	client.allRequest.erase(0, client.allRequest.find("\n") + 1);

	ss << firstLine;

	ss >> client.method;
	ss >> client.url;
	ss >> client.httpVersion;

	if (url.find('?') != std::string::npos)
	{
		client.query = url.substr(url.find('?') + 1);
		url.erase(url.find('?'));
	}

	// erase last / of the url
	if (url != "/" && url[url.size() - 1] == '/')
		url.erase(url.size() - 1, 1);
	
	// std::cout << "method->" << client.method << std::endl;
	// std::cout << "url->" << client.url << std::endl;
	// std::cout << "query->" << client.query << std::endl;
	// std::cout << "HTTP version->" << client.httpVersion << std::endl << std::endl;

	if (client.method.empty())
		client.method = "NONE";

	printHttpMessage(client);
}

void TCPserver::parseChunked(ClientInfo& client, size_t max_body_size)
{
	std::string request = client.allRequest;

	size_t start = 0;

	while ("Mika")
	{
		size_t pos = request.find("\r\n", start);

		std::string hex = request.substr(start, pos - start);

		ssize_t num = hex_to_int(hex);

		if (num <= 0)
		{
			client.chunkedFail = (num == -1);
			return ;
		}

		std::string body = request.substr(start + hex.size() + 2, num);

		start += hex.size() + 2 + num + 2;

		if (start >= request.size())
		{
			client.chunkedFail = true;
			return ;
		}

		client.requestBody += body;

		if (client.requestBody.size() >= max_body_size)
		{
			return ;
		}
	}

}
