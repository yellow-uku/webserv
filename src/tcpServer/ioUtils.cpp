#include "TCPserver.hpp"

int TCPserver::recvfully(int clnt)
{
	int bytes = 0;

	char buf[MAX_BUF + 1];

	bytes = recv(clnt, buf, MAX_BUF, 0);

	if (bytes == MAX_BUF + 1)
		;/// TODO: ban ara

	if(bytes <= 0)
		return bytes;

	buf[bytes] = 0;

	clients[clnt].allRequest = buf;
	parseRequest(clnt);

	return 1;
}

void	TCPserver::sendResponse(int clnt)
{
	// std::cout << "resp - >" << clients[clnt].response.c_str() << std::endl;
	if (send(clnt, clients[clnt].response.c_str(), clients[clnt].response.size(), 0) < 0)
	{
		std::cerr << "Send() failed" << std::endl;
		exit (0); // NEED TO CHANGE!!!! 
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
