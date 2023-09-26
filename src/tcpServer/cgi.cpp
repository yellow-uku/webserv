#include "TCPserver.hpp"

char *ft_strdup(const std::string& s)
{
	char *temp = new char[s.size() + 1];

	for (size_t i = 0; i < s.size(); ++i)
		temp[i] = s[i];

	temp[s.size()] = '\0';
	return temp;
}

char * const *TCPserver::setEnv(std::map<std::string, std::string>& env, const ServerInfo& servData, ClientInfo& client)
{
	env["SERVER_SOFTWARE"] = "webserv/1.0";
	env["SERVER_NAME"] = "localhost";
	env["GATEWAY_INTERFACE"] = "python/69.420";

	//REQUEST_SPECIFIC

	env["SERVER_PROTOCOL"] = "HTTP";
	env["SERVER_PORT"] = servData.socket.port;
	env["REQUEST_METHOD"] = client.method;
	env["PATH_INFO"] = servData.root + client.fullPath;
	env["PATH_TRANSLATED"] = "";
	env["SCRIPT_NAME"] = "";
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

std::string TCPserver::callCgi(const ServerInfo& servData, int client_socket)
{
	const int readEnd = 0;
	const int writeEnd = 1;

	int pipe_to_child[2];
	int pipe_from_child[2];

	if (pipe(pipe_to_child) == -1 || pipe(pipe_from_child) == -1)
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
		std::map<std::string, std::string> env;

		char * const *envp = setEnv(env, servData, clients[client_socket]);

		close(pipe_to_child[writeEnd]);
		close(pipe_from_child[readEnd]);

		dup2(pipe_to_child[readEnd], STDIN_FILENO);
		dup2(pipe_from_child[writeEnd], STDOUT_FILENO);

		std::string scriptPath = (servData.root + clients[client_socket].url);

		char * cgiArgs[] = { const_cast<char *>(servData.cgi.c_str()), const_cast<char *>(scriptPath.c_str()), NULL };

		execve(servData.cgi.c_str(), cgiArgs, envp);

		delete[] envp;

		perror("execve error");
		std::cout << strerror(errno) << "\n";

		exit(1);
	}

	close(pipe_to_child[readEnd]);
	close(pipe_from_child[writeEnd]);

	const char*	requestData = clients[client_socket].requestBody.c_str();

	std::cout <<  "\x1B[32mBuffer: " << clients[client_socket].requestBody.size() << "\x1B[0m\n";

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