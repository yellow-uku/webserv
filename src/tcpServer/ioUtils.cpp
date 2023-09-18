#include "TCPserver.hpp"

int TCPserver::recvfully(int clnt)
{
	ssize_t	bytes = 1;
	ssize_t	max = INT_MAX - 1;

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
		return bytes;

	for (ssize_t i = 0; i < bytes; i++)
	{
		clients[clnt].allRequest.push_back(buff[i]);
	}

	delete[] buff;

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