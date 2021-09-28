#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>

void print_prompt(void)
{
	char* cwd = getcwd(NULL, 0);
	printf("%s", cwd);
	free(cwd);
	printf("$ ");
}

int main(int argc, char** argv)
{
	char* line = NULL;
	size_t line_size = 0;
	while (print_prompt(), 0 <= getline(&line, &line_size, stdin))
	{
		int cmd_argc = 0;
		char* cmd_argv[256];
		cmd_argv[cmd_argc] = strtok(line, " \n");
		while (cmd_argv[++cmd_argc] = strtok(NULL, " \n"))
		{
			assert(cmd_argc<255);
		}

		/*for (int i = 0; i < cmd_argc; i++)
		{
			printf("%d:%s\n", i, cmd_argv[i]);
		}*/

		if (0 == strcmp(cmd_argv[0], "cd"))
		{
			chdir(cmd_argv[1]);
			continue;
		}
		else if (0 == strcmp(cmd_argv[0], "exit"))
		{
			break;
		}
		
		pid_t child_pid = fork();
		assert(child_pid !=-1);

		if (child_pid == 0)
		{
			execvp(cmd_argv[0], cmd_argv);
			fprintf(stderr, "can not find command %s", cmd_argv[0]);
			perror(" ");
			exit(-1);
		}

		int return_code;
		waitpid(child_pid, &return_code,0);
	}
	return 0;
}
