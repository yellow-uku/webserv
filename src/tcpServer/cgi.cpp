#include "TCPserver.hpp"

char *ft_strdup(const std::string& s)
{
	char *temp = new char[s.size() + 1];

	for (size_t i = 0; i < s.size(); ++i)
		temp[i] = s[i];

	temp[s.size()] = '\0';
	return temp;
}

char * const *TCPserver::setEnv(std::map<std::string, std::string>& env, ServerInfo& servData, ClientInfo& client)
{
	env["SERVER_SOFTWARE"] = "webserv/1.0";
	env["SERVER_NAME"] = "localhost";
	env["GATEWAY_INTERFACE"] = "python/69.420";
	env["SERVER_WRITE_PATH"] = servData.uploadDir;
	//REQUEST_SPECIFIC

	env["SERVER_PROTOCOL"] = "HTTP";
	env["SERVER_PORT"] = servData.socket.port;
	env["REQUEST_METHOD"] = client.method;
	env["PATH_INFO"] = servData.root + client.url;
	env["PATH_TRANSLATED"] = servData.root + client.url;
	env["SCRIPT_NAME"] = servData.root + client.url;
	env["QUERY_STRING"] = client.query;
	env["REMOTE_HOST"] = servData.socket.host;
	env["REMOTE_ADDR"] = "";
	env["AUTH_TYPE"] = "";
	env["REMOTE_USER"] = "";
	env["REMOTE_IDENT"] = "";
	env["CONTENT_TYPE"] = client.requestHeaders["Content-Type"];
	env["CONTENT_LENGTH"] = client.requestHeaders["Content-Length"];

	char **envp = new char *[env.size() + 1];

	int i = 0;

	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it)
	{
		std::string val = it->first + "=" + it->second;

		envp[i++] = ft_strdup(val);
	}

	envp[i] = NULL;
	return envp;
}

void TCPserver::callCgi(ServerInfo& servData, ClientInfo& client, std::string& response)
{
	signal(SIGPIPE, SIG_IGN);

	int pipe_to_child[2];
	int pipe_from_child[2];
	pid_t child;

	if (pipe(pipe_to_child) == -1 || pipe(pipe_from_child) == -1 || (child = fork()) == -1)
	{
		perror("pipe error");
		close_pipes(pipe_from_child, pipe_to_child);
		response += readFile(servData.root + servData.error_pages[500]);
		return ;
	}

	fcntl(pipe_to_child[WRITE], F_SETFL, O_NONBLOCK, O_CLOEXEC);
	fcntl(pipe_from_child[READ], F_SETFL, O_NONBLOCK, O_CLOEXEC);

	if (child == 0)
	{
		cgiChild(servData, client, pipe_from_child, pipe_to_child);
	}

	close(pipe_to_child[READ]);
	close(pipe_from_child[WRITE]);

	int res;
	fd_set child_fd;
	struct timeval timeout = {CGI_TIMEOUT, 0};

	FD_ZERO(&child_fd);
	FD_SET(pipe_to_child[WRITE], &child_fd);

	if (client.method == "POST")
	{
		while ((res = select(pipe_to_child[WRITE] + 1, NULL, &child_fd, NULL, &timeout) > 0))
		{
			ssize_t bytes;
			timeout.tv_sec = CGI_TIMEOUT;

			bytes = write(pipe_to_child[WRITE], client.requestBody.c_str(), client.requestBody.size());
			client.requestBody.erase(0, bytes);

			if (client.requestBody.empty())
				break ;
		}

		if (res == 0)
		{
			close(pipe_to_child[WRITE]);
			close(pipe_from_child[READ]);

			kill(child, SIGKILL);
			response = readFile(servData.root + servData.error_pages[408]);

			waitpid(child, NULL, 0);

			std::cout << "--------------------CGI TIMEOUT WRITE--------------------\n";

			return ;
		}
	}

	close(pipe_to_child[WRITE]);

	char c;
	ssize_t bytesRead;

	FD_ZERO(&child_fd);
	FD_SET(pipe_from_child[READ], &child_fd);
	timeout.tv_sec = 8;
	timeout.tv_usec = 0;

	while ((res = select(pipe_from_child[READ] + 1, &child_fd, NULL, NULL, &timeout)) > 0)
	{
		timeout.tv_sec = 5;
		FD_SET(pipe_from_child[READ], &child_fd);

		while ((bytesRead = read(pipe_from_child[READ], &c, 1)) > 0)
		{
			response.push_back(c);
		}

		if (bytesRead == 0)
			break ;
	}

	if (res == 0)
	{
		kill(child, SIGKILL);
		std::cout << "--------------------CGI TIMEOUT READ--------------------\n";
		response = readFile(servData.root + servData.error_pages[408]);
	}

	std::cout << "--------------------CGI END--------------------\n";
	close(pipe_from_child[READ]);

	waitpid(child, NULL, 0);
}

void TCPserver::cgiChild(ServerInfo& servData, ClientInfo& client, int pipe_from_child[2], int pipe_to_child[2])
{
	std::map<std::string, std::string> env;

	char * const *envp = setEnv(env, servData, client);

	dup2(pipe_to_child[READ], STDIN_FILENO);
	dup2(pipe_from_child[WRITE], STDOUT_FILENO);

	close_pipes(pipe_from_child, pipe_to_child);

	std::string scriptPath = (servData.root + client.url);

	char * cgiArgs[] = { const_cast<char *>(servData.cgi.c_str()), const_cast<char *>(scriptPath.c_str()), NULL };

	execve(servData.cgi.c_str(), cgiArgs, envp);

	for (size_t i = 0; envp[i]; ++i)
	{
		delete[] envp[i];
	}

	delete[] envp;

	perror((servData.cgi + scriptPath + ": execve error").c_str());
	std::cout << strerror(errno) << ": " << servData.cgi + scriptPath << "\n";

	exit(1);
}
