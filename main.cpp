#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>



int main()
{
	
	const int	readEnd = 0;
	const int	writeEnd = 1;
	
	// Create a pipe to capture the output of the CGI script
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		perror("pipe");
		return 1;
	}

	// Fork a child process
	pid_t pid = fork();

	if (pid == -1) {
		perror("fork");
		return 1;
	}

	if (pid == 0) { // Child process

		close(pipefd[readEnd]);

		// Redirect stdout to the write end of the pipe
		dup2(pipefd[writeEnd], STDOUT_FILENO);

		// Replace the child process with the process of CGI script
		const char* scriptPath = "hello.py";
		char* const cgiArgs[] = { const_cast<char*>(scriptPath), nullptr };
		execve(scriptPath, cgiArgs, nullptr);

		// If execve fails, print an error message
		perror("execve");
		exit(1);
	} 
	else { // Parent process
	
		close(pipefd[writeEnd]);

		// Read and display the CGI script's output
		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(pipefd[readEnd], buffer, sizeof(buffer))) > 0) {
			write(STDOUT_FILENO, buffer, bytesRead);
		}

		close(pipefd[readEnd]);

		// Wait for the child process to finish
		wait(nullptr);
	}

	return 0;
}