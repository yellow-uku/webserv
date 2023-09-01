# include "TCPserver.hpp"

TCPserver::TCPserver(std::vector< Server_info> data)
{
	myenv = NULL;

	FD_ZERO(&set);
	FD_ZERO(&undo);
	FD_ZERO(&wr_undo);
	FD_ZERO(&wrset);

	createSocketAndAddToSet(data);
}

std::string TCPserver::readFile(std::string filename)
{
	std::fstream indexfile(filename.c_str());
	std::ostringstream sstream;
	sstream << indexfile.rdbuf();
	std::string content(sstream.str());
	return content;
}

TCPserver::~TCPserver(){}

void TCPserver::acceptclnt()
{
	int max_fd;

	struct sockaddr_in clntAddr;
	socklen_t clntAddrlen = sizeof(clntAddr);

	int rc, ret = 0;
	int enable = 1;
	
	std::vector<int> allFd;
	struct Client_info forCleaning;
	

	for(std::vector<int>::iterator it = sock.begin(); it != sock.end(); it++)
	{
		allFd.push_back(*it);
	}

	for(std::vector<int>::iterator it = allFd.begin(); it != allFd.end(); it++)
	{
		if (*it > ret)
			ret = *it;
	}

	max_fd = ret;
	ret = 0;

	while(true)
	{
		FD_ZERO(&set);
		FD_ZERO(&wrset);

		memcpy(&set, &undo, sizeof(set));
		memcpy(&wrset, &wr_undo, sizeof(wrset));

		rc = select(max_fd + 1, &set, &wrset, NULL, NULL);

		std::cout << "select returned, " << rc << "\n";

		if (rc == 0)
			continue ;

		for(int k = 0; k <= max_fd; ++k)
		{
			if (FD_ISSET(k, &wrset))
			{
				sendResponse(k);
				std::cout << "write seti mejic k is ->"<< k << std::endl;
				close (k);
				clients.erase(clients.find(k));
				allFd.erase(std::find(allFd.begin(), allFd.end(), k));
				FD_CLR(k, &wr_undo);
				FD_CLR(k, &undo);
			}
			if (FD_ISSET(k, &set) && std::find(sock.begin(), sock.end(), k) == sock.end())
			{
				std::cout << "read seti mejic k is ->"<< k << std::endl;
				ret = recvfully(k);
				std::cout << "line - >" << __LINE__ << std::endl;

				if (ret <= 0)
				{
					close (k);
					clients.erase(k);
					allFd.erase(std::find(allFd.begin(), allFd.end(), k));
					FD_CLR(k, &wr_undo);
					FD_CLR(k, &undo);
					std::cout << "ret <= 0\n";
					break ;
				}
				std::cout << "line - >" << __LINE__ << std::endl;
				setResponseFile(k);

				FD_SET(k, &wr_undo);
				FD_CLR(k, &undo);
			}

			std::cout << (FD_ISSET(k, &set)) << "  asdas \n";

			if (FD_ISSET(k, &set) && std::find(sock.begin(), sock.end(), k) != sock.end())
			{
				std::cout << "serveri acceptic seti mejic kkkkk is ->"<< k	 << std::endl;
				int clnt = accept(k, (struct sockaddr *)&clntAddr, &clntAddrlen);
				std::cout << "serveri acceptic seti mejic clnt is ->"<< clnt << std::endl;

				if (clnt > max_fd)
					max_fd = clnt;
				allFd.push_back(clnt);

				FD_SET(clnt, &undo);

				if (clnt < 0)
				{
					std::cerr << "accept failed -> " << errno << std::endl;
					perror("accept");
					exit (0);
				}

				fcntl(clnt, F_SETFL, O_NONBLOCK);
			}
			std::cout << "----------------mid-------------" << k << std::endl;
		}
	}
}

bool customFind(std::string &str, char c)
{
	int len = str.length();
	for (int i = 0;i < len; i++)
	{
		if ( str[i] == c)
			return true;
	}
	return false;
}

void TCPserver::setUrl_and_Method(int i)
{
	std::stringstream	ss;
	std::string 		url;
	std::string 		uri;
	std::string 		method;

	ss << clients[i].reqstFirstline;
	std::getline(ss, method, ' ');
	if(customFind(clients[i].reqstFirstline, '?'))
	{
		std::cout << "---------------------------" << std::endl;
		std::getline(ss, url, '?');
		std::getline(ss, uri, ' ');
	}
	else
	{
		// ss >> usr_data.url;
		//  ss.get();
		// std::cout << "------->" << ss.str();
		std::getline(ss, url, ' ');
		uri = " ";
	}
	if(url[0] == '/')
		url = std::string(url.c_str() + 1);
	clients[i].method = method;
	clients[i].url = url;
	clients[i].uri = uri;
	std::cout << "method->" << method << std::endl;
	std::cout << "url->" << url << std::endl;
	std::cout << "uri->" << uri << std::endl;
	
}

std::string TCPserver::readLine(std::string &all,size_t &start)
{
	size_t len;
	if (start == 0)
		len = all.find("\n");
	else
		len = all.find("\n",start);
	std::string retVal;
	if (len == std::string::npos)
		return all;
	len = len - start + 1;
	retVal =  all.substr(start,len);
	start += len;	
	return retVal;
}

bool TCPserver::findKeyValue(std::string &line, size_t index)
{
	size_t len = line.find(":");
	std::string kval, vval;
	if (len == std::string::npos)
		return false;
	kval =  line.substr(0,len);
	len++;
	vval = line.substr(len,line.find("\n"));
	clients[index].requestHeaders[kval] = vval;
	return true;
}

void	TCPserver::parseRequest(std::string &content, int i)
{
	std::string key, value;
	// std::getline(content, reqstFirstline);
	size_t  beg = 0 ;
	std::string line;
	clients[i].reqstFirstline = readLine(content, beg);
	while(true)
	{
		line = readLine(content, beg);
		if(!findKeyValue(line,i))
			break;
	}
	std::cout << "my var -> " << beg << " actual size ->" << clients[i].allRequest.size() << "\n" << content <<  std::endl;
	if (beg < content.size() - 1)
	{
		clients[i].requestBody = clients[i].allRequest.substr(beg);
		std::cout << "substrrrrr" << std::endl;
	}
	else
	{
		clients[i].requestBody = "a";
		std::cout << "vasvsacasdasd" << std::endl;
	}
	// std::cout << clients[i].requestBody;
	// std::cout << "client body ------------->\n" <<  clients[i].requestBody  << "\n end of body----<"<< std::endl;

	// for(std::map<std::string,std::string>::iterator it = clients[i].requestHeaders.begin(); it != clients[i].requestHeaders.end();it++)
	// {
	// 	std::cout << "key ->" << it->first << "\nvalue ->" << it->second << std::endl;
	// }
	// std::cout << "vvv->" << clients[i].requestHeaders["Host"] << std::endl;
	setUrl_and_Method(i);
}

int TCPserver::recvfully(int clnt)
{
	int bytes = 0;
	std::string rt = "";
	std::vector<char> buff(1024*1024);

	bytes = recv(clnt,&buff[0],buff.size(),0); //bufsize
	if(bytes <= 0)
		return bytes;
	buff[bytes] = '\0';
	for(std::vector<char>::iterator it = buff.begin(); it != buff.end(); it++)
	{
		bytes--;
		rt += *it;
		if (bytes == -1)
			break;
	}
	// std::cout << "recieved ->" << rt << "endl--->" << std::endl;
	clients[clnt].allRequest = std::string(rt);
	parseRequest(rt, clnt);
	return 1;
}
/*
  	HTTP/1.1 200 OK\r\n"
	"Connection: Close\r\n"
	"Content-Type: text/css\r\n"
	"\r\n"
*/
std::string	TCPserver::find_and_set_cont_type(int i)
{
	if (clients[i].url.find(".css") != std::string::npos)
		return (std::string("text/css"));
	else if (clients[i].url.find(".jpg") != std::string::npos)
		return (std::string("image/jpeg"));

	//typeri checky avelacnel
	return (std::string("text/html"));
}

bool	TCPserver::thereIsNoIndexFile(Server_info &servData)
{
	return servData.index_files.size() == 0;
}

std::string TCPserver::correctIndexFile(std::string &fileName, Server_info &servData)
{
	std::string full_path;
	for (std::vector<std::string>::iterator it = servData.index_files.begin(); it < servData.index_files.end(); ++it)
	{
		full_path = fileName + *it;
		if (access(full_path.c_str(), F_OK & R_OK) == 0)
			return full_path;
	}

	return 	servData.root + "/" + servData.error_pages[404];
}

bool	TCPserver::isLocation(Server_info &info,std::string &name)
{
	return info.location.count(name);
}

 Server_info TCPserver::correctInfos( Server_info &infos, std::string & name)
{
	return infos.location[name];
}

bool TCPserver::isRedirect(Response_headers &headData, Server_info &servData, int i)
{
	std::cout << "mtav mejy" << std::endl;
	if (servData.redirect[0] == '/')
		servData.redirect.erase(0,1);
	std::cout << "url ->" << clients[i].url << " --> redirect" << servData.redirect << std::endl;
	if(!servData.redirect.size() || clients[i].url == servData.redirect)
		return false;
	(void) headData;

	clients[i].response = "HTTP/1.1 307 Temporary Redirect\r\n"
	"HTTP/1.1 307 Temporary Redirect\r\n"
	"Location: " + servData.redirect +"\r\n"
	"Content-Length: 0\r\n"
	"Connection: close\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n"
	"\r\n";
	return true;
}

bool TCPserver::checkDir(std::string &dirName,Response_headers & heading, Server_info &servData)
{
	struct stat fs;
	size_t len, start = 1;
	bool 	forwhile = true;
	std::string dName;
	
	std::cout << "chediri dursn" << std::endl;
	std::cout << "mode" << std::bitset<2>(fs.st_mode) << std::endl;
	std::cout << "macro" << std::bitset<2>(S_IXOTH) << std::endl;
	
	while(forwhile)
	{
		len = dirName.find("/",start);
		if (len == std::string::npos)
		{
			dName = dirName;
			forwhile = false;
		}
		else
			dName = dirName.substr(0,len);

		std::cout << len << " dir name ->" << dName << std::endl;
		start = len + 1;
		stat(dName.c_str(),&fs);

		if (!(S_IXOTH & fs.st_mode))
		{
			std::cout << "chediri mejna" << std::endl;
			dirName = servData.root + "/" + servData.error_pages[403];
			heading.http_status = "403";
			return true;
		}
	}
	return false;
}

bool TCPserver::checkFile(std::string &fileName,Response_headers & heading, Server_info &servData)
{
	struct stat fs;
	stat(fileName.c_str(),&fs);
	std::cout << "chefile dursn" << std::endl;

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

void	TCPserver::buildResponse(std::string &fileName,Response_headers & heading, bool dir, int i)
{
	std::string response;
	std::string headers;
	
	if (!dir)
	{
		heading.content_type = find_and_set_cont_type(i);
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		response += readFile(fileName);
		clients[i].full_path = fileName;
		clients[i].response = response;
	}
	else
	{
		heading.build_headers();
		headers = heading.headers;
		response = headers;
		response += listDir(fileName);
		clients[i].response = response;
		clients[i].full_path = fileName;
	}
}

bool TCPserver::checkMethod(std::string &fileName,Response_headers &heading,  Server_info &servData,int i)
{
	for (std::vector<std::string>::iterator it = servData.allow_methods.begin(); it != servData.allow_methods.end(); ++it)
	{
		if (*it == clients[i].method)
			return true;
	}
	heading.http_status = "405";
	fileName = servData.root + "/" + servData.error_pages[405];
	return false;
}

bool TCPserver::checkBodySize(std::string &fileName,Response_headers &heading,  Server_info &servData,int i)
{	
	std::cout << "sizeeeee ->" << clients[i].requestBody.length() << "<--" << std::endl;
	if(clients[i].requestBody.size() > servData.max_body_size)
	{
		heading.http_status = "413";
		fileName = servData.root + "/" + servData.error_pages[413];
		return true;
	}
	return false;
}

void	TCPserver::setResponseFile(int i)
{
	std::string			fileName;
	std::string			response;
	Server_info			servData;
	Response_headers	heading;
	short		portN = 0;

	portN = atoi(clients[i].requestHeaders["Host"].c_str() + clients[i].requestHeaders["Host"].find(':') + 1);
	std::cout << "port number" << portN  << "\n\n\n"<< std::endl;
	if (isLocation(server_data[portN],clients[i].url))
	{
		servData = correctInfos(server_data[portN],clients[i].url);
	}
	else
	{
		servData = server_data[portN];
	}
	
	if (servData.root[servData.root.size() - 1] != '/')
		servData.root += "/";
	heading.http_status = "200";
	if (isRedirect(heading,servData,i))
		return ;
	
	fileName = servData.root + clients[i].url;
	std::cout << "fileName - > " << fileName << std::endl;
	if(!checkMethod(fileName,heading,servData,i) || checkBodySize(fileName,heading,servData,i))
	{
		buildResponse(fileName,heading, 0,i);
		return;
	}
	if (isDir(fileName))
	{
		if (checkDir(fileName,heading,servData))
		{
			buildResponse(fileName,heading, 0, i);
			return;
		}
		if (!servData.autoindex)
		{
			if (thereIsNoIndexFile(servData))
			{
				// std::cout << "filename ->>>>>> > " << fileName << std::endl;
				// heading.http_status = "403";
				// fileName = servData.root + "/" + servData.error_pages[403];//403 Forbidden dnel es
				fileName = "";
			}
			else
			{
				fileName = correctIndexFile(fileName,servData);
				std::cout << "filename ->>>>>> vvvvvvvvvvvvv> " << fileName << std::endl;
			}
		}
		else
		{
			buildResponse(fileName,heading, 1, i);
			return ;
		}
	}
	checkFile(fileName,heading,servData);// body,methods 
	buildResponse(fileName,heading, 0,i);
	return ;
}

bool TCPserver::isDir(std::string &name)
{
	DIR *folder;
	folder = opendir(name.c_str());
	if(folder == NULL)
	{
		// std::cout << name << " aaaaaaaaaaaaaaaaaaaaaaa\n";
		return false;
	}
	if (name[name.size() - 1] != '/')
		name += "/";
	closedir(folder);
	return true;
}

std::string TCPserver::listDir(std::string &name)
{
	DIR *folder;
	struct dirent *entry;
	// //std::cout << "//////////// " << name << "////////////" << std::endl; 
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
		std::cerr << "Error occured" << std::endl;
		return std::string("not found");
	}
	while( (entry=readdir(folder)) )
	{
		htmlCode = htmlCode + "<li>" + entry->d_name + "</li>\r\n";
	}
	closedir(folder);
	htmlCode = htmlCode +  "</ul>\r\n" + "</body>\r\n" + "</html>\r\n";
	return htmlCode;
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

unsigned int TCPserver::url_lenght(std::string &str)
{
	size_t len = str.find('?');
	if (len == std::string::npos)
		return str.size();
	return (len);
}

void	TCPserver::parseFirstLine(std::string line)
{
	std::string 		row;
	std::string 		token;
	std::stringstream	stream(line);
	std::getline(stream,row);
	//somechecks
	std::stringstream tokens(row);
	tokens >> token;
}

//response headers

Response_headers::Response_headers()
{
	http_versus	= "HTTP/1.1 ";
	http_status	= "200 OK";
	connection	= "Close";
	content_type	= "text/html";
	backslash_r_n	= "\r\n";
}

Response_headers::~Response_headers()
{}

void Response_headers::build_headers()
{
	headers = http_versus + http_status + backslash_r_n 
		+ "Connection: " + connection + backslash_r_n
		+ "Content-Type: " + content_type + backslash_r_n
		+ backslash_r_n;
	//std::cout << "*************\n" << headers << std::endl;
}
