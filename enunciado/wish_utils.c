#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"

void execute_exit(int value)
{
	exit(0);
}

void execute_cd(char *newpath)
{
	char *path = strtok_r(newpath, " ", &newpath);
	int result = chdir(path);

    if (result != 0) {
        write(STDERR_FILENO, error_message, strlen(error_message));
    }
}

void execute_path(char *path_string)
{
	char *path;
	char **new_path_array;
	int i, path_count;

	path = strtok_r(path_string, " ", &path_string);
	path_count = 0;
	while (path)
	{
		path_count++;
		path = strtok_r(NULL, " ", &path_string);
	}

	new_path_array = malloc(sizeof(char *) * (path_count + 1));
	new_path_array[path_count] = NULL;

	path = strtok_r(path_string, " ", &path_string);
	i = 0;
	while (path)
	{
		new_path_array[i] = malloc(sizeof(char) * (strlen(path) + 1));
		strcpy(new_path_array[i], path);
		path = strtok_r(NULL, " ", &path_string);
		i++;
	}

	// for (i = 0; i < path_count; i++)
	// {
	// 	free(mypath[i]);
	// }
	// free(mypath);

	i = 0;
	while (new_path_array[i] != NULL)
	{
		strcpy(mypath[i], new_path_array[i]);
		i++;
	}
	//mypath[i] = "";

	//mypath = new_path_array;

	printf("path executed\n");
}
