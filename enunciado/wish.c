#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"

#define MAX_SIZE 500

int main(int argc, char *argv[])
{

	char str[MAX_SIZE];
	char *command_string;
	char *s;
	int fd;

	char **mypath = malloc(2 * sizeof(char *));
	mypath[0] = "/bin/";
	mypath[1] = "";

	do
	{
		printf("wish> ");
		fgets(str, MAX_SIZE, stdin);
		s = str;
		while (*s != '\n')
		{
			++s;
		}
		*s = '\0';
		s = str;
		command_string = strtok_r(s, " ", &s);

		printf("command_string es solo el comando sin argumentos: (%s)\n", command_string);
		printf("La variable s tiene sólo los argumentos del comando y son: (%s)\n", s);

		int i = 0;
		printf("-------------------mypath--------------\n");
		while (strcmp(mypath[i],"") != 0)
		{
			printf("%s\n", mypath[i]);
			i++;
		}
		printf("--------------------------------\n");

		// Comandos Build-in
		if (strcmp(command_string, "exit") == 0)
		{
			execute_exit(s);
		}
		else if (strcmp(command_string, "cd") == 0)
		{
			execute_cd(s);
		}
		else if (strcmp(command_string, "path") == 0)
		{
			execute_path(s, &mypath);

			// Ejecución de otro programa, no Build-On
		}
		else
		{
			fd = -1;
			char **mp = mypath;
			char specificpath[MAX_SIZE];

			// De acuerdo a las rutas especificadas en mypath, comprobaremos cuales son accesibles y correspondientes al ejecutable,
			// Por ejemplo si queremos usar el comando ls, y en mypath está su ruta /bin/, entonces deberíamos de obtener en specificpath /bin/ls
			// ls es como cualquier programa ejecutable escrito en c o un script, con o sin argumentos

			printf("-------------specificpath-------------------\n");
			while ((strcmp(*mp, "") != 0) && fd != 0)
			{
				strcpy(specificpath, *mp++);
				strncat(specificpath, command_string, strlen(command_string));
				fd = access(specificpath, X_OK);
				printf("ruta valida? %d\n", fd);
				printf("specificpath %s\n\n", specificpath);
			}
			printf("--------------------------------\n");

			// Si el file descriptor existe, osea la ruta del programa y el programa
			if (fd == 0)
			{
				// Como comprobamos que el programa existe, entonces creamos un proceso hijo
				int subprocess = fork();

				// Error lanzando el subproceso
				if (subprocess < 0)
				{
					printf("Error launching the subprocess");
				}
				else if (subprocess == 0)
				{ // Estoy en el proceso hijo
					// Como al hacer fork se hace una copia exacta e independiente de todo el programa padre, puedo manipular variables en el entorno del hijo
					// Extraigo los argumentos para preparar el execv()

					// Cuento cuantos argumentos tiene el comando
					int num_args = 0;
					char *s_copy = strdup(s);
					char *token = strtok(s_copy, " ");
					while (token != NULL)
					{
						num_args++;
						token = strtok(NULL, " ");
					}
					printf("num_args: %d\n", num_args);

					// Guardo los argumentos en el vector myargs
					int i = 1;
					char *myargs[num_args + 1];
					myargs[0] = strdup(command_string);

					char *s_copy2 = strdup(s);
					token = strtok(s_copy2, " ");
					while (token != NULL)
					{
						myargs[i] = token;
						token = strtok(NULL, " ");
						i++;
					}
					myargs[i] = NULL;

					i = 0;
					printf("------------MYARGS------------\n");
					while (myargs[i] != NULL)
					{
						printf("(%s)\n", myargs[i]);
						i++;
					}
					printf("--------------------------------\n");

					// Como me interesa ejecutar el proceso hijo como un nuevo programa, mando los argumentos capturados en el comando sobreescribiendo la imagen del proceso en cuestión
					// Se hace mediente la función execv(), toma 2 argumentos: el primero es la ruta donde se encuentra el ejecutable y el segundo son el nombre de ejecutable con sus argumentos en caso de tenerlos
					// y es necesario especificar null al final de los argumentos
					// Así si se ejecuta el comando "ls -la", la funcion debe ser: execv("/bin/ls", { "ls", "-la", NULL})

					execv(specificpath, myargs);
				}
				else
				{ // Estoy en el proceso padre y esperaré a que los hijos terminen

					wait(NULL);
				}
			}
			else
			{ // Si el file descriptor no existe
				printf("Command not found: %s\n", str);
			}
		}
	} while (1);
}
