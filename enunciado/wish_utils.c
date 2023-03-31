#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

extern char *mypath[];
extern char error_message[30];

void execute_exit(char *args)
{
	exit(0);
}

/*
void execute_cd(char *newpath){
	char *path = strtok_r(newpath, " ", &newpath);
	chdir(path);
}

*/

// Implementación del comando cd para cambiar la ruta del directorio actual,sólo debe recibir 1 argumento que será el cambio a la nnueva ruta
void execute_cd(char *newpath)
{
	printf("Variable newpath %s\n", newpath);
	char *path = strtok_r(newpath, " ", &newpath);
	if (path == NULL)
	{
		printf("Error: el comando cd requiere un argumento\n");
	}
	else
	{
		if (strtok_r(NULL, " ", &newpath) != NULL)
		{
			printf("Error: el comando cd solo acepta un argumento\n");
		}
		else
		{
			if (access(path, F_OK) == 0)
			{
				chdir(path);
				printf("La nueva ruta es %s\n", path);
			}
			else
			{
				printf("Directory does not exist: %s\n", path);
			}
		}
	}
}

// Implementación del comando path que contiene las rutas donde se buscarán los comandos ejecutables en el programa,
// puede recibir cero(Se conserva el path actual) o más argumentos(se actualiza el path actual con el numero de argumentos que se ingresen)
void execute_path(char *newpath, char ***mypath)
{
	char *copypath = strdup(newpath);
	int path_count = 0;
	int i = 0;
	char *path = strtok_r(copypath, " ", &copypath);

	// count number of paths to be added
	while (path != NULL)
	{
		path_count++;
		path = strtok_r(copypath, " ", &copypath);
	}
	// Path con cero argumentos
	if (path_count == 0)
	{

		printf("Variable mypath sin modificaciones\n");
	}
	else
	{ // Path con n argumentos
		*mypath = realloc(*mypath, (path_count + 1) * sizeof(char *));

		path = strtok_r(newpath, " ", &newpath);
		i = 0;
		while (path != NULL)
		{
			(*mypath)[i] = strdup(path);
			path = strtok_r(newpath, " ", &newpath);
			i++;
		}
		// set last element of mypath to ""
		(*mypath)[i] = "";
	}
}

int wish_launch_redirect(char **args, char *file)
{
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
	pid_t pid, wpid;
	int status;
	pid = fork();
	if (pid == 0)
	{
		// Child process
		int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

		dup2(fd, 1); // make stdout go to file
		dup2(fd, 2); // make stderr go to file - you may choose to not do this
					 // or perhaps send stderr to another file
		close(fd);
		execv(args[0], args);
	}
	else if (pid < 0)
	{
		// Error forking
		write(STDERR_FILENO, error_message, strlen(error_message));
	}
	else
	{
		// Parent process
		wait(NULL);
	}
	return 1;
}