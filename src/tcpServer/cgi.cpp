#include "TCPserver.hpp"

std::string TCPserver::callCgi(const ServerInfo& servData, int client_socket)
{
	const int	readEnd = 0;
	const int	writeEnd = 1;

	int pipe_to_child[2];
	int pipe_from_child[2];

	if (pipe(pipe_to_child) == -1 or pipe(pipe_from_child) == -1)  {
		perror("pipe error");
		return "";
	}

	pid_t child = fork();

	if (child == -1)
	{
		perror("fork error");
		return "";
	}

	if (child == 0)
	{
		close(pipe_to_child[writeEnd]);
		close(pipe_from_child[readEnd]);

		dup2(pipe_to_child[readEnd], STDIN_FILENO);
		dup2(pipe_from_child[writeEnd], STDOUT_FILENO);

		std::string scriptPath = (servData.root + clients[client_socket].url);

		char * cgiArgs[] = { const_cast<char *>(scriptPath.c_str()), nullptr };

		execve(scriptPath.c_str(), cgiArgs, nullptr);

		perror("execve error");

		std::cout << scriptPath.c_str() << "\n";

		exit(1);
	} 
	else
	{
	
		close(pipe_to_child[readEnd]);
		close(pipe_from_child[writeEnd]);

		const char*	requestData = clients[client_socket].requestBody.c_str();

		write(pipe_to_child[writeEnd], requestData, strlen(requestData));
		close(pipe_to_child[writeEnd]);

		char c;
		std::string buffer;
		ssize_t bytesRead;

		while ((bytesRead = read(pipe_from_child[readEnd], &c, 1)) > 0)
		{
			buffer.push_back(c);
		}

		close(pipe_from_child[readEnd]);

		std::cout << "\x1B[35m" <<  buffer << "\x1B[0m\n";

		std::cout << clients[client_socket].requestBody << "------------------------\n";

		waitpid(child, NULL, 0);

		return buffer;
	}

	return "";
}