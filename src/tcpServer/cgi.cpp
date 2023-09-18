#include "TCPserver.hpp"

std::string TCPserver::callCgi(const ServerInfo& servData, int client_socket)
{
	const int	readEnd = 0;
	const int	writeEnd = 1;

	int pipe_to_child[2];
	int pipe_from_child[2];

	if (pipe(pipe_to_child) == -1 or pipe(pipe_from_child) == -1)
	{
		perror("pipe error");
		return strerror(errno);
	}

	pid_t child = fork();

	if (child == -1)
	{
		perror("fork error");
		return strerror(errno);
	}

	if (child == 0)
	{
		std::map<std::string,std::string>  customEnv;
		std::stringstream sstr;
		std::string key, val,newstr;
		for(int i = 0; myEnv[i]; i++)
		{
			newstr += (std::string(myEnv[i]) + "\n");
		}
		sstr << std::string(newstr);
		for(int i = 0;myEnv[i]; ++i)
		{
			std::getline(sstr,key,'=');
			std::getline(sstr,val);
			customEnv[key] = val;
		}
		char **newEnv = (char **)malloc(sizeof(char * ) * (customEnv.size() + 7));
		int z = 0;
		for(std::map<std::string, std::string>::iterator it = customEnv.begin(); it != customEnv.end(); ++it)
		{
			newEnv[z] = strdup((it->first + "=" + it->second).c_str());
			z++;
		}
		const char *c = "CONTENT_TYPE=text/html";
		const char *l = "PATH_INFO=/Users/vagevorg/Desktop/webserv/www/cgi_bin/hello.py";
		const char *t = "SCRIPT_FILENAME=hello.py";
		const char *e = "REQUEST_METHOD=post";
		const char *b = "CONTENT_LENGTH=500";

		std::cout << customEnv.size() << "----" << z << "\n";
		newEnv[z++] = strdup(c);
		newEnv[z++] = strdup(l);
		newEnv[z++] = strdup(t);
		newEnv[z++] = strdup(e);
		newEnv[z++] = strdup(b);
		newEnv[z] = 0;
		// for(int i = 0;myEnv[i]; ++i)
		// {
		// 	std::cout << myEnv[i] << "a2aaa\n";
		// }
		close(pipe_to_child[writeEnd]);
		close(pipe_from_child[readEnd]);

		dup2(pipe_to_child[readEnd], STDIN_FILENO);
		dup2(pipe_from_child[writeEnd], STDOUT_FILENO);

		std::string scriptPath = (servData.root + clients[client_socket].url);

		char * cgiArgs[] = { const_cast<char *>(scriptPath.c_str()), NULL };

		// execve(scriptPath.c_str(), cgiArgs, newEnv);
		execve("./www/cgi_bin/hello.py", cgiArgs, newEnv);

		perror("execve error");

		std::cout << strerror(errno) << "\n";

		exit(1);
	}
	else
	{
		close(pipe_to_child[readEnd]);
		close(pipe_from_child[writeEnd]);

		const char*	requestData = clients[client_socket].requestBody.c_str();

		std::cout <<  "\x1B[32mBuffer: " << clients[client_socket].requestBody.size() << "\x1B[0m\n";

		// for(size_t i = 0; i < clients[client_socket].requestBody.size(); i++)
		// {
		// 	std::cout << clients[client_socket].requestBody[i];
		// }
		std::cout << "\n";

		write(pipe_to_child[writeEnd], requestData, clients[client_socket].requestBody.size());
		close(pipe_to_child[writeEnd]);

		char c;
		std::string buffer;
		ssize_t bytesRead;

		while ((bytesRead = read(pipe_from_child[readEnd], &c, 1)) > 0)
		{
			buffer.push_back(c);
		}

		close(pipe_from_child[readEnd]);

		std::cout << "\x1B[35mBuffer: " <<  buffer << "\x1B[0m\n";

		waitpid(child, NULL, 0);

		return buffer;
	}

	return "";
}

// #include "TCPserver.hpp"

// std::string TCPserver::callCgi(const ServerInfo& servData, int client_socket)
// {
// 	const int	readEnd = 0;
// 	const int	writeEnd = 1;

// 	int pipe_to_child[2];
// 	int pipe_from_child[2];

// 	if (pipe(pipe_to_child) == -1 or pipe(pipe_from_child) == -1)
// 	{
// 		perror("pipe error");
// 		return strerror(errno);
// 	}

// 	pid_t child = fork();

// 	if (child == -1)
// 	{
// 		perror("fork error");
// 		return strerror(errno);
// 	}

// 	if (child == 0)
// 	{
// 		close(pipe_to_child[writeEnd]);
// 		close(pipe_from_child[readEnd]);

// 		dup2(pipe_to_child[readEnd], STDIN_FILENO);
// 		dup2(pipe_from_child[writeEnd], STDOUT_FILENO);

// 		std::string scriptPath = (servData.root + clients[client_socket].url);

// 		char * cgiArgs[] = { const_cast<char *>(scriptPath.c_str()), NULL };

// 		execve(scriptPath.c_str(), cgiArgs, NULL);

// 		perror("execve error");

// 		std::cout << strerror(errno) << "\n";

// 		exit(1);
// 	}
// 	else
// 	{
// 		close(pipe_to_child[readEnd]);
// 		close(pipe_from_child[writeEnd]);

// 		const char*	requestData = clients[client_socket].requestBody.c_str();

// 		write(pipe_to_child[writeEnd], requestData, strlen(requestData));
// 		close(pipe_to_child[writeEnd]);

// 		char c;
// 		std::string buffer;
// 		ssize_t bytesRead;

// 		while ((bytesRead = read(pipe_from_child[readEnd], &c, 1)) > 0)
// 		{
// 			buffer.push_back(c);
// 		}

// 		close(pipe_from_child[readEnd]);

// 		std::cout << "\x1B[35mBuffer: " <<  buffer << "\x1B[0m\n";

// 		waitpid(child, NULL, 0);

// 		return buffer;
// 	}

// 	return "";
// }