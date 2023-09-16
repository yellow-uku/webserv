#include "TCPserver.hpp"

size_t TCPserver::maxBodySize()
{
	size_t max = 0;

	for (size_t i = 0; i < serverData.size(); ++i)
	{
		if (serverData[i].info.max_body_size > max)
			max = serverData[i].info.max_body_size;
	}

	return max;
}

int TCPserver::recvfully(int clnt)
{
	int		bytes = 0;
	size_t	body_size = maxBodySize();
	size_t	max = (body_size == 0 ? (body_size) : MAX_BODY_SIZE_K);
	
	max = max * 2; // for headers and body

	char	*buff;
	
	try
	{
		buff = new char[max];
	}
	catch(const std::exception& e)
	{
		std::cerr << "new[] failed: " << e.what() << '\n';
		return -1;
	}

	bytes = recv(clnt, buff, max + 1, 0);

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
