#include "TCPserver.hpp"

std::string	TCPserver::getBoundary(std::string contentType, ResponseHeaders& headers)
{
	size_t index = contentType.find("boundary=");

	if (index == contentType.npos)
	{
		return "";
	}

	return (contentType.substr(index + 9));
}

void TCPserver::postMultipart(std::string requestBody, std::string boundary, ResponseHeaders &headers)
{
	size_t final = requestBody.find(boundary + "--");
	if (final == requestBody.npos)
	{
		headers.http_status = "400";
		return ;
	}

	size_t	start_index = 0;
	for (;;)
	{
		size_t start = requestBody.find(boundary , start_index) + boundary.size();

		if (requestBody.substr(start, 2) == "--")
			break ;

		size_t end = requestBody.find(boundary, start);

		std::string temp = requestBody.substr(start, end - start + 1);

		std::string a = temp.substr(temp.find("\r\n\r\n") + 4);

		std::fstream file((my_to_string(time(NULL)) + ".png").c_str(), std::ios::out | std::ios::binary);
	
		file << a;

		std::cout << "|" << a << "|" << std::endl;

		start_index = end;
	}
}

void TCPserver::postImage()
{

}

void TCPserver::parsePostRequest(ClientInfo& client, ResponseHeaders& headers)
{
	std::string type = client.requestHeaders["Content-Type"];
	std::string boundary = "--" + getBoundary(type, headers);

	if (boundary == "--" || !contains(ClientInfo::allowed_content_type, type))
		headers.http_status = "400";
}
