#include "TCPserver.hpp"

int TCPserver::receive(ClientInfo& client, int clnt, socket_t& socket)
{
	ssize_t	bytes = 1;
	ssize_t	max = 150000 - 1;

	char *buf;

	try
	{
		buf = new char[max + 1];
	}
	catch(const std::exception& e)
	{
		std::cerr << "new[] failed: " << strerror(errno) << '\n';
		return -1;
	}

	bytes = recv(clnt, buf, max, 0);

	if (bytes <= 0)
	{
		delete[] buf;
		return bytes;
	}

	std::cout << "socket " << clnt << " performed a read: " << buf << "\n";

 	// std::cout << "Bytes: " << bytes << "\n";

	socket.timeout = time(NULL);

	for (ssize_t i = 0; i < bytes; i++)
	{
		client.allRequest.push_back(buf[i]);
	}

	delete[] buf;

	if (client.method.empty() && client.allRequest.find("\n"))
	{
		setUrlAndMethod(client);
	}

	if (client.requestHeaders.size() == 0 && client.allRequest.find("\r\n\r\n"))
	{
		parseRequest(client);

		if (client.method != "POST")
			return 1;
	}

	if (!client.requestHeaders["Content-Length"].empty())
	{
		ServerInfo& servData = getLocationData(socket, client.requestHeaders["Host"], client.url);

		if (client.allRequest.size() >= servData.max_body_size
			|| client.allRequest.size() == my_stos_t(client.requestHeaders["Content-Length"]))
		{
			client.requestBody = client.allRequest;
			return 1;
		}
	}
	else if (client.requestHeaders["Transfer-Encoding"] == "chunked")
	{
		std::cout << "ischunked\n";
		if (client.allRequest.find("\r\n0\r\n") != std::string::npos)
		{
			ServerInfo& servData = getLocationData(socket, client.requestHeaders["Host"], client.url);

			parseChunked(client, servData.max_body_size);
			return 1;
		}
	}
	else
		return 1; // error, no length

	return 2;
}

bool TCPserver::sendResponse(int clnt)
{
	ssize_t len = 0;
	ssize_t size = clients[clnt].response.size();

	if ((len = send(clnt, clients[clnt].response.c_str(), clients[clnt].response.size(), 0)) < 0)
	{
		perror("Send");
	}

	clients[clnt].response.erase(0, len);

	return len == size;
}

std::string TCPserver::readFile(std::string filename)
{
	std::fstream indexfile(filename.c_str());
	std::ostringstream sstream;

	sstream << indexfile.rdbuf();

	return sstream.str();
}