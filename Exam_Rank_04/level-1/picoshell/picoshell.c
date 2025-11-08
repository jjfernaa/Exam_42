#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int	picoshell(char **cmds[])
{
	int		i = 0;
	int	 	prev_fd = -1;
	int	 	pipefd[2];
	pid_t	pid;
	int		status;
	while (cmds[i])
	{
		if (cmds[i + 1])
		{
			if (pipe(pipefd) == -1)
				return (1);
		}
		pid = fork();
		if (pid == -1)
			retunr (1);
		if (pid == 0)
		{
			if (prev_fd != -1)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}	
			if(cmds[i +1])
			{
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit (1);
		}
		else
		{
			if(prev_fd != -1)
				close(prev_fd);
			if(cmds[i +1])
			{
				close(pipefd[1]);
				prev_fd = pipefd[0];
			}
			else
			{
				close(pipefd[0]);
				close(pipefd[1]);
			}
		}
		i++;
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			return (1);
		if (!WIFEXITED(status))
			return (1);
	}
	return (0);
}

