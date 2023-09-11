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

	int pipe_to_child[2];
	int pipe_from_child[2];

	if (pipe(pipe_to_child) == -1 or pipe(pipe_from_child) == -1)  {
		perror("pipe error");
		return 1;
	}

	// Fork a child process
	pid_t child = fork();

	if (child == -1) {
		perror("fork error");
		return 1;
	}

	if (child == 0) { // Child process

		// Close unnecessary ends of the pipe
		close(pipe_to_child[writeEnd]);
		close(pipe_from_child[readEnd]);

		// Redirect stdin and stdout to the pipes
		dup2(pipe_to_child[readEnd], STDIN_FILENO);
		dup2(pipe_from_child[writeEnd], STDOUT_FILENO);

		// Replace the child process with the process of CGI script
		const char* scriptPath = "hello.py";
		char* const cgiArgs[] = { const_cast<char*>(scriptPath), nullptr };
		
		if (execve(scriptPath, cgiArgs, nullptr) == -1)
			perror("execve error");

		exit(1);
	} 
	else if (child > 0)
	{ // Parent process
	
		close(pipe_to_child[readEnd]);
		close(pipe_from_child[writeEnd]);

		// Send data to the child process
		const char*	requestData = "example_data\n";
		write(pipe_to_child[writeEnd], requestData, strlen(requestData));
		close(pipe_to_child[writeEnd]); // Close the write end to signal the end of data

		// Read and display the CGI script's output
		char buffer[1024];
		ssize_t bytesRead;
		while ((bytesRead = read(pipe_from_child[readEnd], buffer, sizeof(buffer))) > 0) {
			if (write(STDOUT_FILENO, buffer, bytesRead) == -1)
				perror("write to CGI error");
		}

		close(pipe_from_child[readEnd]);

		// Wait for the child process to finish
		waitpid(child, NULL, 0);
	}
	else
		perror("fork doesn't work");

	return 0;
}