#include "TCPserver.hpp"

int TCPserver::recvfully(int clnt)
{
	int bytes = 0;

	// std::vector<char> buff(MAX_BUF);
	char *buff = new char[MAX_BUF];

	bytes = recv(clnt, &buff[0], MAX_BUF + 1, 0);

	if (bytes <= 0)
		return bytes;

	buff[bytes] = '\0';

	for (ssize_t i = 0; i < bytes; i++)
	{
		clients[clnt].allRequest.push_back(buff[i]);
	}

	delete[] buff;

	parseRequest(clnt);

	return 1;
}

void	TCPserver::sendResponse(int clnt)
{
	if (send(clnt, clients[clnt].response.c_str(), clients[clnt].response.size(), 0) < 0)
	{
		perror("Send");
	}
}

std::string TCPserver::readFile(std::string filename)
{
	std::fstream indexfile(filename.c_str());
	std::ostringstream sstream;

	sstream << indexfile.rdbuf();

	return sstream.str();
}
// #include "TCPserver.hpp"

// int TCPserver::recvfully(int clnt)
// {
	// char *buff = new char[1024 * 1024 * 1024];

// 	ssize_t bytes = recv(clnt, buff, (1024 * 1024 * 1024) + 1, 0);

// 	if (bytes <= 0)
// 		return bytes;


// 	buff[bytes] = '\0';

// 	std::cout << buff << "          ------------   >>>>>\n";

// 	clients[clnt].allRequest = buff;
// 	delete[] buff;

// 	parseRequest(clnt);

// 	return 1;
// }

// void	TCPserver::sendResponse(int clnt)
// {
// 	if (send(clnt, clients[clnt].response.c_str(), clients[clnt].response.size(), 0) < 0)
// 	{
// 		perror("Send");
// 	}
// }

// std::string TCPserver::readFile(std::string filename)
// {
// 	std::fstream indexfile(filename.c_str());
// 	std::ostringstream sstream;

// 	sstream << indexfile.rdbuf();

// 	return sstream.str();
// }
