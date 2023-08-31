# include "../headers/TCPsocket.hpp"
# include <algorithm>
#include <sys/types.h>
	#include <sys/stat.h>
#include <unistd.h>
#include <bitset>
TCPserver::TCPserver(std::vector<struct server_infos> data)
{
	myenv=NULL;
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

TCPserver::~TCPserver()
{
}

int	TCPserver::acceptclnt()
{
	struct sockaddr_in clntAddr;
	int	clnt;
	char clntaddress[INET_ADDRSTRLEN];
	socklen_t clntAddrlen = sizeof(clntAddr);
	int rc, ret = 0;
	int k = 0;
	std::vector<int> allFd;
	struct client_info forCleaning;
	struct timeval myTime;

	(void) rc;
	//std::cout << "---------------------start-----------" << std::endl;
	for(std::vector<int>::iterator it = sock.begin(); it != sock.end(); it++)
	{
		allFd.push_back(*it);
	}
	int enable;
	while(1)
	{
	myTime.tv_sec = 0;
	myTime.tv_usec = 0;
	// bzero(&clntAddr,sizeof(struct sockaddr_in));
	ret = 0;
	k = 0;
	clnt = 0;
	for(std::vector<int>::iterator it = allFd.begin(); it != allFd.end(); it++)
	{
		if (*it > ret)
			ret = *it;
	}
	max_fd = ret + 1;
	ret = 0;
	FD_ZERO(&set);
	FD_ZERO(&wrset);
	memcpy(&set, &undo, sizeof(set));
	memcpy(&wrset, &wr_undo, sizeof(wrset));
	// std::cout << "select blocks" << std::endl;
	rc = select(max_fd, &set, &wrset, NULL,&myTime);
	// std::cout << "select unblocks" << std::endl;
	if (rc > 0)
	std::cout << "select return value" << rc << "maxfd "  << max_fd << std::endl;
	if (rc == 0)
		continue;
	enable = 1;
	while(k < max_fd) // <=
	{
	if (FD_ISSET(k, &wrset))// && std::find(sock.begin(), sock.end(), k) == sock.end())
	{
		// std::cout << "full response " << clients[k].response << std::endl;
		sendResponse(k);
		std::cout << "write seti mejic k is ->"<< k << std::endl;
		close (k);
		clients.erase(clients.find(k));
		allFd.erase(std::find(allFd.begin(), allFd.end(), k));
		FD_CLR(k, &wr_undo); //wr_undo
		FD_CLR(k, &undo); //wr_undo
		break;//pordzenq
	}
	else if (FD_ISSET(k, &set) && std::find(sock.begin(), sock.end(), k) == sock.end())
	{
		std::cout << "read seti mejic k is ->"<< k << std::endl;
		ret = recvfully(k);
		std::cout << "line - >" << __LINE__ << std::endl; 	
		// std::cout << "recieved" << std::endl;
		if (ret <= 0)
		{
			close (k);
			clients.erase(clients.find(k));
			allFd.erase(std::find(allFd.begin(), allFd.end(), k));
			FD_CLR(k, &wr_undo); //wr_undo
			break;
		}
		std::cout << "line - >" << __LINE__ << std::endl;
		setResponseFile(k);
		// FD_CLR(k, &undo); //wr_undo
		FD_SET(k, &wr_undo);
		FD_SET(k, &wrset);
		FD_CLR(k, &undo); //wr_undo
		// std::cout << "requested-path" << clients[k].full_path << std::endl;
		break;//pordzenq

	}
	if (FD_ISSET(k,&set) && std::find(sock.begin(), sock.end(), k) != sock.end())
	{
		std::cout << "serveri acceptic seti mejic kkkkk is ->"<< k	 << std::endl;
		clnt = accept(k, (struct sockaddr *)&clntAddr, &clntAddrlen);
		std::cout << "serveri acceptic seti mejic clnt is ->"<< clnt	 << std::endl;
		if (clnt > max_fd)
			max_fd = clnt;
		allFd.push_back(clnt);
		// FD_SET(clnt, &wr_undo);
		// FD_SET(clnt, &wrset);
			FD_SET(clnt, &set);
			FD_SET(clnt, &undo);

		//std::cout << "added" << std::endl;
		if (clnt < 0)
		{
			std::cerr << "accept failed -> " << errno << std::endl;
			perror("aa");
			exit (0);
		}
		if(inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntaddress, sizeof(clntaddress)) == NULL)
		{
			std::cerr << "ntop failed" << std::endl;
			exit (0);
		}
		
        fcntl(clnt, F_SETFL, O_NONBLOCK); //hishi baces es

		if (setsockopt(clnt, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
                perror("setsockopt(SO_REUSEADDR) failed");
                exit(EXIT_FAILURE);
        }
		std::cout << "Connected -->" << clntaddress << "<--" << std::endl;
		break;//pordzenq

	}
	//std::cout << "----------------mid-------------" << k << std::endl;

	k++;
	}
	//std::cout << "----------------end-------------" << std::endl;
	}
	return clnt;
}
bool customFind(std::string &str,char c)
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
	std::stringstream ss;
	std::string url;
	std::string uri;
	std::string method;
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
	if(url.front() == '/')
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
	{
		return (std::string("text/css"));
	}
	else if (clients[i].url.find(".jpg") != std::string::npos)
	{
		return (std::string("image/jpeg"));

	}
	//typeri checky avelacnel
	return (std::string("text/html"));
}

bool	TCPserver::thereIsNoIndexFile(struct server_infos &servData)
{
	if (servData.index_files.size() == 0)
		return true;
	else
		return false;
}

std::string TCPserver::correctIndexFile(std::string &fileName, struct server_infos &servData)
{
	std::string full_path;
	for (std::vector<std::string>::iterator it = servData.index_files.begin(); it < servData.index_files.end(); ++it)
	{
		full_path = fileName + *it;
		if (access(full_path.c_str(), F_OK & R_OK) == 0)
		{
			return full_path;
		}
	}
	return 	servData.root + "/" + servData.error_pages[404];

}

bool	TCPserver::isLocation(struct server_infos &info,std::string &name)
{
	if (info.location.count(name))
	{
		return true;
	}
	else
		return false;
}

struct server_infos TCPserver::correctInfos(struct server_infos &infos, std::string & name)
{
	// tpel name vor tenanq parsingi het brnum a te che
	return infos.location[name];
}

bool 	TCPserver::isRedirect(response_headers &headData, struct server_infos &servData, int i)
{
	std::cout << "mtav mejy" << std::endl;
	if (servData.redirect.front() == '/')
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

bool TCPserver::checkDir(std::string &dirName,response_headers & heading,struct server_infos &servData)
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
		if(!(S_IXOTH & fs.st_mode))
		{
			std::cout << "chediri mejna" << std::endl;
			dirName = servData.root + "/" + servData.error_pages[403];
			heading.http_status = "403";
			return true;
		}
	}
	return false;
}

bool TCPserver::checkFile(std::string &fileName,response_headers & heading,struct server_infos &servData)
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

void	TCPserver::buildResponse(std::string &fileName,response_headers & heading, bool dir, int i)
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

bool TCPserver::checkMethod(std::string &fileName,response_headers &heading, struct server_infos &servData,int i)
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

bool TCPserver::checkBodySize(std::string &fileName,response_headers &heading, struct server_infos &servData,int i)
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
	std::string fileName;
	std::string response;
	struct server_infos servData;
	short		portN = 0;
	response_headers heading;
	portN = atoi(clients[i].requestHeaders["Host"].c_str() + clients[i].requestHeaders["Host"].find(':') + 1);
	std::cout << "port number" << portN  << "\n\n\n"<< std::endl;
	if (isLocation(server_data[portN],clients[i].url))
	{
		servData = correctInfos(server_data[portN],clients[i].url);
	}
	else{
		servData = server_data[portN];
	}
	if (servData.root.back() != '/')
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
		std::cout << name << " aaaaaaaaaaaaaaaaaaaaaaa\n";
		return false;
    }
	if (name.back() != '/')
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
		exit (0); // ushadir poxelu es sa 
	}
}
unsigned int TCPserver::url_lenght(std::string &str)
{
	size_t len = str.find('?');
	if (len == std::string::npos)
		return str.size();;
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
