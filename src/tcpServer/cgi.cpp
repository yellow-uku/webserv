#include "TCPserver.hpp"

std::string TCPserver::callCgi(const ServerInfo& servData, int client_socket)
{
	const int	readEnd = 0;
	const int	writeEnd = 1;
	
	// Create a pipe to capture the output of the CGI script
	int pipefd[2];

	int pipe_to_child[2];
	int pipe_from_child[2];

	if (pipe(pipe_to_child) == -1 or pipe(pipe_from_child) == -1)  {
		perror("pipe error");
		return "";
	}

	// Fork a child process
	pid_t child = fork();

	if (child == -1)
	{
		perror("fork error");
		return "";
	}

	if (child == 0)
	{ // Child process

		// Close unnecessary ends of the pipe
		close(pipe_to_child[writeEnd]);
		close(pipe_from_child[readEnd]);

		// Redirect stdin and stdout to the pipes
		dup2(pipe_to_child[readEnd], STDIN_FILENO);
		dup2(pipe_from_child[writeEnd], STDOUT_FILENO);

		// Replace the child process with the process of CGI script

		std::string scriptPath = (servData.root + clients[client_socket].url);

		char * cgiArgs[] = { const_cast<char *>(scriptPath.c_str()), nullptr };

		execve(scriptPath.c_str(), cgiArgs, nullptr);

		// close(pipe_to_child[readEnd]);
		perror("execve error");

		std::cout << scriptPath.c_str() << "\n";

		exit(1);
	} 
	else if (child > 0)
	{ // Parent process
	
		close(pipe_to_child[readEnd]);
		close(pipe_from_child[writeEnd]);

		// Send data to the child process
		const char*	requestData = clients[client_socket].requestBody.c_str();

		write(pipe_to_child[writeEnd], requestData, strlen(requestData));
		close(pipe_to_child[writeEnd]); // Close the write end to signal the end of data

		// Read and display the CGI script's output
		char buffer[1024];
		ssize_t bytesRead;

		ssize_t length = 0;

		while ((bytesRead = read(pipe_from_child[readEnd], buffer, sizeof(buffer))) > 0) {
			if (write(STDOUT_FILENO, buffer, bytesRead) == -1)
				perror("write to CGI error");
			length += bytesRead;
		}

		buffer[length] = 0;

		std::cout << buffer << "  buffer \n";

		close(pipe_from_child[readEnd]);

		// Wait for the child process to finish
		waitpid(child, NULL, 0);

		return std::string(buffer);
	}
	else
		perror("fork doesn't work");

	return "";
}