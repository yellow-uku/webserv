#include "TCPserver.hpp"

const char *ClientInfo::allowed_content_type[] = {
	"image/jpeg",
	"image/png",
	"multipart/form-data",
	"text/plain",
	NULL,
};

void TCPserver::parsePostRequest(ClientInfo& client, ResponseHeaders& headers, std::string& type)
{
	type = client.requestHeaders["Content-Type"];
	client.boundary = "--" + getBoundary(type);

	type = type.find(';') != std::string::npos ? type.substr(0, type.find(';')) : type;

	if (!contains(ClientInfo::allowed_content_type, type)
		|| (type == "multipart/form-data" && (client.boundary == "--"
		|| client.requestBody.find(client.boundary + "--") == std::string::npos)))
			headers.http_status = "400";
}

std::string	TCPserver::getBoundary(std::string contentType)
{
	size_t index = contentType.find("boundary=");

	if (index == contentType.npos)
		return "";

	return (contentType.substr(index + 9));
}

bool TCPserver::postMultipart(std::string& requestBody, std::string& boundary, std::string& upload)
{
	size_t start_index = 0;

	int err = 0;
	bool fail = false;

	while (1)
	{
		size_t start = requestBody.find(boundary , start_index) + boundary.size();

		if (requestBody.substr(start, 2) == "--")
			break ;

		size_t end = requestBody.find(boundary, start);

		std::string temp = requestBody.substr(start, end - start);

		std::string headers = temp.substr(0, temp.find("\r\n\r\n"));

		std::string body = temp.substr(temp.find("\r\n\r\n") + 4);

		if ((err = parseBody(body, headers, upload)))
		{
			std::cerr << "File upload error: " << strerror(err) << "\n";
			fail = true;
		}

		start_index = end;
	}

	return fail;
}

int TCPserver::post(std::string& type, std::string &body, std::string& upload)
{
	std::string extension;

	if (type == "image/jpeg")
		extension = ".jpg";
	else if (type == "image/png")
		extension = ".png";
	else if (type == "text/plain")
		extension = ".txt";

	if (std::find_if(body.begin(), body.end(), IsNotSpace()) == body.end())
		return 0;

	std::string filename = "download";

	std::string temp = filename;

	for (size_t i = 1; access((upload + filename + extension).c_str(), F_OK) == 0; ++i)
	{
		filename = temp + " (" + my_to_string(i) + ")";
	}

	std::fstream file((upload + filename + extension).c_str(), std::ios::out | std::ios::binary);

	if (file.fail())
	{
		perror(("Fstream: " + upload + filename + extension).c_str());
		return errno;
	}

	file << body;

	return 0;
}

int TCPserver::parseBody(std::string& body, std::string& headers, std::string& upload)
{
	size_t filenamePos = headers.find("filename=\"") == std::string::npos ? std::string::npos : headers.find("filename=\"") + 10;

	size_t endPos = headers.find('\"', filenamePos);

	std::string filename = (filenamePos == std::string::npos ? "" : headers.substr(filenamePos, endPos - filenamePos));

	size_t contentPos = ((headers.find("Content-Type:") == std::string::npos) ? std::string::npos : headers.find("Content-Type:") + 13);

	while (contentPos != std::string::npos && std::isspace(headers[contentPos++])) ;

	std::string type = (contentPos == std::string::npos ? "" : headers.substr(contentPos - 1, headers.find("\r\n", contentPos) - contentPos));

	return uploadFile(filename, type, body, upload);
}

int TCPserver::uploadFile(std::string& filename, std::string& type, std::string& body, std::string& upload)
{
	std::string extension = ".mdd";
	size_t extensionPos = filename.rfind('.');

	if (extensionPos != std::string::npos && extensionPos != filename.size() - 1)
		extension = filename.substr(extensionPos);
	else if (type == "image/jpeg")
		extension = ".jpg";
	else if (type == "image/png")
		extension = ".png";

	if (std::find_if(body.begin(), body.end(), IsNotSpace()) == body.end())
		return 0;

	filename = extensionPos == std::string::npos ? filename : filename.substr(0, filename.rfind('.'));

	if (filename.empty())
		filename = "download";

	std::string temp = filename;

	for (size_t i = 1; access((upload + filename + extension).c_str(), F_OK) == 0; ++i)
	{
		filename = temp + " (" + my_to_string(i) + ")";
	}

	std::fstream file((upload + filename + extension).c_str(), std::ios::out | std::ios::binary);

	if (file.fail())
		return errno;

	file << body;

	return 0;
}
