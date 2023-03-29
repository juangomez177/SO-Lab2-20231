#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"

#define MAX_SIZE 100

char *mypath[] = {};

int main(int argc, char *argv[])
{

	char str[MAX_SIZE];
	char *command_string;
	char *s;
	int fd;
	strcpy(mypath[0],"/bin");
	mypath[1]=NULL;

	do
	{
		printf("whish> ");
		fgets(str, MAX_SIZE, stdin);
		s = str;
		while (*s != '\n')
		{
			++s;
		}
		*s = '\0';
		s = str;
		command_string = strtok_r(s, " ", &s);

		if (strcmp(command_string, "exit") == 0)
		{
			execute_exit(0);
		}
		else if (strcmp(command_string, "cd") == 0)
		{
			execute_cd(s);
		}
		else if (strcmp(command_string, "path") == 0)
		{
			execute_path(s);
		}
		else
		{
			fd = -1;
			char **mp = mypath;
			char specificpath[MAX_SIZE];
			while ((strcmp(*mp, "") != 0) && fd != 0)
			{
				strcpy(specificpath, *mp++);
				strncat(specificpath, command_string, strlen(command_string));
				fd = access(specificpath, X_OK);
			}
			if (fd == 0)
			{
				int subprocess = fork();
				if (subprocess < 0) // error
				{
					printf("Error launching the subprocess");
				}
				else if (subprocess == 0) // subproceso hijo
				{
					char *myargs[3];
					myargs[0] = strdup(specificpath);
					myargs[1] = strdup(".");
					myargs[2] = NULL;
					execvp(myargs[0], myargs);
				}
				else
				{
					wait(NULL);
				}
			}
			else
			{
				printf("Command not found: %s\n", str);
			}
		}
	} while (1);
}
