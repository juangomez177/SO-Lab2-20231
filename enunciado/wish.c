/*
	Ejecucion de multiples subprocesos con batch e interactivo mode
	Para probar, puede usar como ejemplo "ls & pwd & ls -la"

	para compilar: gcc -o wish wish.c wish_utils.c -lreadline
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wish_utils.h"
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_COMMANDS 1000
#define MAX_SIZE 500
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 30
//#define DELIMITERS " \t\r\n\a\0"

char error_message[30] = "An error has occurred\n";
char history[HISTORY_SIZE][BUFFER_SIZE];
int history_count = 0;

void procesar_comando(char *command, char ***mypath)
{	int num_com = 0;
	int fd;

	//Comandos Built-In, se validará que no se ingresen comandos como "cd /bin/ & path /bin/", ya que es incorrecto para comandos Built-In
	if ((strstr(command, "cd") != 0 || strstr(command, "path") != 0 || strstr(command, "exit") != 0)&& strstr(command, "&") == 0) {
		printf("Comando tipo Built.in'\n");

		//Separamos del comando original el nombre del comando y sus argumentos
		char *s = command;
		char *command_string = strtok_r(s, " ", &s);
		
		printf("El comando original: (%s)\n", command);
		printf("command_string es solo el comando sin argumentos: (%s)\n", command_string);
		printf("La variable s tiene sólo los argumentos y son: (%s)\n\n", s);
	
		if(strcmp(command_string, "exit") == 0){
			execute_exit(s);

		}
		else if(strcmp(command_string, "cd") == 0){
			execute_cd(s);

		}
		else if(strcmp(command_string, "path") == 0){
			execute_path(s, mypath);

		}
	}
	//Ejecución de otros comandos, no Built-In. Puede ser 1 o varios, donde serán delimitados por el "&"
	else if (strstr(command, "cd") == NULL && strstr(command, "path") == NULL && strstr(command, "exit") == NULL){
		printf("Comando tipo No built.in\n");

		//Extraccion de todos los posibles comandos
		num_com=0;
		char *command_copy = strdup(command);
		char *token = strtok(command_copy, "&");
		while (token != NULL) {
			//Verificamos que el comando no esté vacío
			token += strspn(token, " ");
			if (*token != '\0') {
				num_com++;
			}
			token = strtok(NULL, "&");
		}
		printf("num_com: %d\n", num_com);
				
		char *all_commands[num_com+1];
		char *command_copy2 = strdup(command);
		token= strtok(command_copy2, "&");
		int i=0;
		while (token != NULL) {
			///Verificamos que el comando no esté vacío
			token += strspn(token, " ");
			if (*token != '\0') {
				all_commands[i] = token;
				i++;
			}
			token = strtok(NULL, "&");
		}
		all_commands[i]=NULL;
		
		i=0;
		printf("------------all_commands------------\n");
		while(all_commands[i]!=NULL){
			printf("(%s)\n", all_commands[i]);
			i++;
		}
		printf("--------------------------------\n");

		//Inicializamos los pids de los comandos capturados
		pid_t pids[num_com];
			
		//Recorrido para todos los comandos que serán lanzados
		for(i=0; i<num_com; i++){

			//Separamos del comando original el nombre del comando y sus argumentos
			char *s = all_commands[i];
			char *command_string = strtok_r(s, " ", &s);

			printf("\n--------Comando a procesar: (%s) ---------\n", all_commands[i]);
			printf("El comando original: (%s)\n", all_commands[i]);
			printf("command_string es solo el comando sin argumentos: (%s)\n", command_string);
			printf("La variable s tiene sólo los argumentos y son: (%s)\n\n", s);

			//Iniciamos comprobación del file descriptor basado en buscar las rutas del comando
			fd = -1;
			char ***mp_copy = malloc(2 * sizeof(char **)); // Crea una copia de mypath
			memcpy(mp_copy, mypath, 2 * sizeof(char **));
			char ***mp = mp_copy;
			char specificpath[MAX_SIZE];

			printf("\n-------------specificpath para (%s)---------------\n", all_commands[i]);
			while ((strcmp((*mp)[0], "") != 0) && fd != 0){
				strcpy(specificpath, *(mp[0]++));
				strncat(specificpath, command_string, strlen(command_string));
				fd = access(specificpath, X_OK);
				printf("ruta valida? %d\n", fd);
				printf("specificpath %s\n\n", specificpath);
			}
			printf("--------------------------------\n");

			// Si el file descriptor existe, osea la ruta del programa 
			if (fd == 0){
				printf("entra FD correcto: %d\n", fd);
				//Lanzaremos cada comando como un nuevo proceso

				//En caso de error
				if ((pids[i] = fork()) < 0) {
					printf("Error launching the subprocess\n");
					perror("fork");
					abort();

				
				}//Proceso hijo creado
				else if(pids[i] == 0){
					printf("\n-------Estoy en un proceso hijo y voy a ejecutar: (%s)-------\n", all_commands[i]);
					
					//Como al hacer fork se hace una copia exacta e independiente de todo el programa padre, puedo manipular variables en el entorno del hijo
					//Extraigo los argumentos para preparar el execv()

					//Conteo de los argumentos del comando
					int num_args = 0;
					char *s_copy = strdup(s);
					char *token = strtok(s_copy, " ");
					while (token != NULL) {
						num_args++;
						token = strtok(NULL, " ");
					}
					printf("num_args: %d\n", num_args);
					
					//Guardo los argumentos en el vector myargs
					int i = 1;
					char *myargs[num_args+1];
					myargs[0] = strdup(command_string);
					
					char *s_copy2 = strdup(s);
					token= strtok(s_copy2, " ");
					while (token != NULL) {
						myargs[i] = token;
						token = strtok(NULL, " ");
						i++;
					}
					myargs[i] = NULL;
										
					i=0;
					printf("------------MYARGS------------\n");
					while(myargs[i]!=NULL){
						printf("(%s)\n", myargs[i]);
						i++;
					}
					printf("--------------------------------\n\n");
					
					//Lanzo el proceso hijo como un nuevo programa
					//Como me interesa ejecutar el proceso hijo como un nuevo programa, mando los argumentos capturados en el comando sobreescribiendo la imagen del proceso en cuestión
					execv(specificpath, myargs);

					//Si execv() es exitoso, lo siguiente nunca se ejecutará, por precaución para una salida segura
					printf("Error al ejecutar execvp\n");
					exit(EXIT_FAILURE);

				}//Fin del proceso hijo
			}//Si el file descriptor no existe
			else{
				printf("Command not found: %s\n\n", all_commands[i]);
			}
		}//Fin del for

		//Esperamos que todos los hijos terminen
		int status;
		pid_t pid;
		for(i=0; i<num_com; i++){
			pid = wait(&status);
			printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
			printf("El proceso  %s ha terminado correctamente\n", all_commands[i]);
			printf("--------------------------------\n\n");
		}
	}//Comando incorrecto
	else{
		printf("Comando incorrecto %s\n", command);
	}
}

int main(int argc, char *argv[])
{
	//Inicialización del mypath
	char **mypath = malloc(2 * sizeof(char *));
	mypath[0] = "/bin/";
	mypath[1] = "";

	//Para capturar la entrada
	char *input_line;

	// Modo interactivo
	if (argc == 1)
	{
		
		do
		{

			input_line = readline("wish> ");
			if (!input_line)
			{
				// EOF o error
				break;
			}
			if (strlen(input_line) > 0)
			{
				add_history(input_line);

				// Copiar cadena de comando al búfer de historial
				if (history_count < HISTORY_SIZE)
				{
					strcpy(history[history_count++], input_line);
				}
				else
				{
					for (int i = 0; i < HISTORY_SIZE - 1; i++)
					{
						strcpy(history[i], history[i + 1]);
					}
					strcpy(history[HISTORY_SIZE - 1], input_line);
				}
			}

			//Parseo de la entrada capturada con el fin de secuencia
			input_line[strcspn(input_line, "\n")] = '\0';

			// Ejecuto el comando
			printf("COMANDO: %s\n", input_line);
			procesar_comando(input_line, &mypath);

		} while (1);
		free(input_line);
	}else if (argc == 2) // Modo batch
	{
		char commands[MAX_COMMANDS][MAX_SIZE];
		int num_commands = 0;

		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL)
		{
			printf("Error opening file\n");
			exit(1);
		}

		// Leer lineas del archivo
		while (fgets(commands[num_commands], MAX_SIZE, fp))
		{
			num_commands++;
		}
		fclose(fp);

		// Ejecutar los comandos en orden
		for (int i = 0; i < num_commands; i++)
		{
			input_line = commands[i];

			//Parseo de la entrada capturada con el fin de secuencia
			input_line[strcspn(input_line, "\n")] = '\0';
			
			// Ejecuto el comando
			printf("COMANDO: %s\n", input_line);
			procesar_comando(input_line, &mypath);
		}
	}
	else if(argc >1)
	{
		write(STDERR_FILENO, error_message, strlen(error_message)); 
		
	}

	return 0;
}