# include "../headers/TCPsocket.hpp"

void	TCPserver::createSocket(int port)
{
	int	portNum;
	portNum = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (portNum < 0)
        {
                std::cerr << "Fail socket" << std::endl;
                exit(0);
        }
        struct sockaddr_in servAddr;
        bzero(&servAddr, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(port);
        if (bind(portNum, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
        {
                std::cerr << "Bind failed" << std::endl;
                perror("bind failed. Error");
                exit(0);
        }
        if (listen(portNum, 500) < 0)
        {
                std::cerr << "Listen failed" << std::endl;
                exit (0);
        }
        const int enable = 1;
        if (setsockopt(portNum, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
                perror("setsockopt(SO_REUSEADDR) failed");
                exit(EXIT_FAILURE);
        }
         int reuseport = 1;
        if (setsockopt(portNum, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport)) < 0) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
        }
        fcntl(portNum, F_SETFL, O_NONBLOCK);
	FD_SET(portNum,&set);
        FD_SET(portNum,&undo);
	max_fd = portNum + 1;
	sock.push_back(portNum);

}


////////////////////////////////////////////////////////


void	TCPserver::createSocketAndAddToSet(std::vector<struct server_infos> &data){

        struct server_infos forLoc;
        forLoc.root = "../../locWeb";
        forLoc.error_pages[404] = "404.html";
        forLoc.index_files.push_back("index.html");
        FD_ZERO(&undo);
        FD_ZERO(&wr_undo);
	for(std::vector<struct server_infos>::iterator it = data.begin();it != data.end();it++)
	{
                // forLoc.redirect = "http://localhost:" + std::to_string(it->port) + "/specify.html";
                // forLoc.redirect = "/specify.html";
                forLoc.redirect = "/barev/";
		createSocket(it->port);
                server_data[it->port] = *it;
                server_data[it->port].error_pages[404] = "404.html";
                server_data[it->port].error_pages[403] = "403.html";
                server_data[it->port].error_pages[413] = "413.html";
                server_data[it->port].error_pages[405] = "405.html";
                server_data[it->port].root = std::string("../../web");
                server_data[it->port].autoindex = false;
                server_data[it->port].max_body_size = 1024;
                server_data[it->port].location["barev"] = forLoc;
                server_data[it->port].allow_methods.push_back("GET");
                server_data[it->port].allow_methods.push_back("POST");
                
	}
}
