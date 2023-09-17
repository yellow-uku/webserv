#include "TCPserver.hpp"

size_t TCPserver::maxBodySize()
{
	size_t max = 0;

	for (size_t i = 0; i < serverData.size(); ++i)
	{
		if (serverData[i].info.max_body_size > max)
		{
			// std::cout << serverData[i].info.max_body_size << "\n";
			max = serverData[i].info.max_body_size;
		}
	}

	return ((max >= INT_MAX || max == 0) ? INT_MAX - 1 : max);
}

int TCPserver::recvfully(int clnt)
{
	// int bytes = 0;
	// std::string rt = "";
	// std::vector<char> buff(1024*1024);

	// bytes = recv(clnt,&buff[0],buff.size(),0); //bufsize
	// if(bytes <= 0)
	// 	return bytes;
	// buff[bytes] = '\0';
	// for(std::vector<char>::iterator it = buff.begin(); it != buff.end(); it++)
	// {
	// 	bytes--;
	// 	rt += *it;
	// 	if (bytes == -1)
	// 		break;
	// }
	// // std::cout << "recieved ->" << rt << "endl--->" << std::endl;
	// clients[clnt].allRequest = std::string(rt);
	// parseRequest(clnt);
	// return 1;
	ssize_t	bytes = 1;
	ssize_t	max = maxBodySize();

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

	if (bytes <= 0 || bytes == max + 1)
		return bytes;

	for (ssize_t i = 0; i < bytes; i++)
	{
		clients[clnt].allRequest.push_back(buff[i]);
		// std::cout << buff[i];
	}
	// std::cout << std::endl;

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
