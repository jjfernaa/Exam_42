#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int ft_popen(const char *file, char *const argv[], char type)
{
	int		pipefd[2];
	pid_t	pid;

	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(pipefd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		if (type == 'r')
		{
			close(pipefd[0]);
			if (dup2(pipefd[1], STDOUT_FILENO) == -1)
				return (-1);
			close(pipefd[1]);
		}
		else
		{
			close(pipefd[1]);
			if (dup2(pipefd[0], STDIN_FILENO) == -1)
				return (-1);
			close(pipefd[0]);
		}
		execvp(file, argv);
		exit(1);
	}
	else
	{
		if (type == 'r')
		{
			close(pipefd[1]);
			return (pipefd[0]);
		}
		else
		{
			close(pipefd[0]);
			return (pipefd[1]);
		}
	}
	return (0);
}
