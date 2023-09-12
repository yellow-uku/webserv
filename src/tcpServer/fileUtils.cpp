#include "TCPserver.hpp"

bool TCPserver::checkFile(std::string &fileName, ResponseHeaders &heading, ServerInfo &servData)
{
	struct stat fs;
	stat(fileName.c_str(),&fs);

	if(access(fileName.c_str(), F_OK) == -1)
	{
		fileName = servData.root + "/" + servData.error_pages[404];
		heading.http_status = "404";
		return true;
	}
	else if(!(S_IROTH & fs.st_mode))
	{
		fileName = servData.root + "/" + servData.error_pages[403];
		heading.http_status = "403";
		return true;
	}
	return false;
}

bool TCPserver::checkDir(std::string &dirName, ResponseHeaders &heading, ServerInfo &servData)
{
	struct stat fs;
	size_t len, start = 1;
	bool 	forwhile = true;
	std::string dName;

	// while(forwhile)
	// {
	// 	len = dirName.find("/",start);
	// 	if (len == std::string::npos)
	// 	{
	// 		dName = dirName;
	// 		forwhile = false;
	// 	}
	// 	else
	// 		dName = dirName.substr(0,len);
	// 	start = len + 1;

	stat(dirName.c_str(), &fs);

	if (!(S_IXOTH & fs.st_mode))
	{
		dirName = servData.root + "/" + servData.error_pages[403];
		heading.http_status = "403";
		return true;
	}
	// } didn't work on my linux
	return false;
}

bool TCPserver::isDir(std::string &name)
{
	DIR *folder;

	folder = opendir(name.c_str());

	if (folder == NULL)
		return false;

	if (name[name.size() - 1] != '/')
		name += "/";

	closedir(folder);
	return true;
}

std::string TCPserver::listDir(std::string &name)
{
	DIR *folder;
	struct dirent *entry;
	std::string htmlCode = "<!DOCTYPE html>\r\n"
							"<html lang='en'>\r\n"
							"<head>\r\n"
							"<meta charset='UTF-8'>\r\n"
							"<meta name='viewport' content='width=device-width, initial-scale=1.0'>\r\n"
							"<title>Directory listing</title>\r\n"
							"</head>\r\n"
							"<body>\r\n"
							"<ul>\r\n";

	folder = opendir(name.c_str());

	if(folder == NULL)
	{
		perror("Opendir");
		return "not found";
	}

	while( (entry = readdir(folder)) )
	{
		htmlCode = htmlCode + "<li>" + entry->d_name + "</li>\r\n";
	}

	closedir(folder);
	htmlCode = htmlCode +  "</ul>\r\n" + "</body>\r\n" + "</html>\r\n";
	return htmlCode;
}
