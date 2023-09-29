#include "TCPserver.hpp"

int TCPserver::receive(ClientInfo& client, int clnt)
{
	ssize_t	bytes = 1;
	ssize_t	max = 150000 - 1;

	char *buff;

	try
	{
		buff = new char[max + 1];
	}
	catch(const std::exception& e)
	{
		std::cerr << "new[] failed: " << strerror(errno) << '\n';
		return -1;
	}

	bytes = recv(clnt, buff, max, 0);

	std::cout << "Bytes: " << bytes << "\n";

	if (bytes <= 0)
		return -1;

	for (ssize_t i = 0; i < bytes; i++)
	{
		client.allRequest.push_back(buff[i]);
	}

	delete[] buff;

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
		if (client.allRequest.size() == my_stos_t(client.requestHeaders["Content-Length"]))
		{
			client.requestBody = client.allRequest;
			return 1;
		}
	}
	else if (client.requestHeaders["Transfer-Encoding"] == "chunked")
	{
		// parse chunked request;
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