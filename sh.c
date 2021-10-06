#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/reboot.h>
#include <search.h>
#include <readline/readline.h>

char* get_prompt(void)
{
	static char prompt[128];
	char* cwd = getcwd(NULL, 0);
	sprintf(prompt, "%s $", cwd);
	free(cwd);
	return prompt;
}

int cd_func(int argc, char**argv);
int exit_func(int argc, char** argv);
int printenv_func(int argc, char** argv);

typedef struct
{
	char* cmd;
	int (*func)(int argc, char**argv);
}command_t;

command_t cmd_table[] = {
	{"cd",cd_func},
	{"exit",exit_func},
	{"printenv", printenv_func},
};

int command_name_compare(const void* a, const void* b)
{
	return strcmp(((command_t*)a)->cmd, ((command_t*)b)->cmd);
}
char** environ;
int main(int argc, char** argv, char** envp)
{
	environ = envp;
	char* line = NULL;
	size_t line_size = 0;
	while (printf("%s",get_prompt()), getline(&line, &line_size, stdin))
	{
		if (*line == '\0')continue;
		int cmd_argc = 0;
		char* cmd_argv[256];
		cmd_argv[cmd_argc] = strtok(line, " \n");
		assert(cmd_argv[cmd_argc]);
		while (cmd_argv[++cmd_argc] = strtok(NULL, " \n"))
		{
			assert(cmd_argc<255);
		}

		command_t search_cmd;
		search_cmd.cmd = cmd_argv[0];
		size_t cmd_table_len = sizeof(cmd_table)/sizeof(cmd_table[0]);
		command_t* cmd = lfind(&search_cmd, cmd_table, &cmd_table_len,
			sizeof(cmd_table[0]), command_name_compare);

		if (cmd)
		{
			cmd->func(cmd_argc, cmd_argv);
		}
		if (!cmd)
		{
			pid_t child_pid = fork();
			assert(child_pid !=-1);

			if (child_pid == 0)
			{
				execvp(cmd_argv[0], cmd_argv);
				fprintf(stderr,
					 "can not find command %s", cmd_argv[0]);
				perror(" ");
				exit(-1);
			}

			int return_code;
			waitpid(child_pid, &return_code,0);
		}
	}
	free(line);
	line_size = 0;
	if (getpid() == 1)
	{
		sync();
		reboot(RB_POWER_OFF);
		perror("shutdown fail");
		sleep(10);
	}
	return 0;
}
int cd_func(int argc, char**argv)
{
	chdir(argv[1]);
	return 0;
}
int exit_func(int argc, char** argv)
{
	if (getpid() == 1)
	{
		sync();
		reboot(RB_POWER_OFF);
		perror("shutdown fail");
		sleep(10);
	}
	exit(0);
}
int printenv_func(int argc, char** argv)
{
	for (int i = 0; environ[i]; i++)
	{
		printf("%s\n", environ[i]);
	}
	return 0;
}
