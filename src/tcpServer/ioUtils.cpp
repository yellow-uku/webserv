#include "TCPserver.hpp"

int TCPserver::recvfully(int clnt)
{
	int bytes = 0;

	std::vector<char> buff(1024 * 1024);

	bytes = recv(clnt, &buff[0], buff.size() ,0);

	if (bytes <= 0)
		return bytes;

	buff[bytes] = '\0';

	std::string buf(buff.begin(), buff.end());

	clients[clnt].allRequest = buf;
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
	std::string content(sstream.str());

	return content;
}
