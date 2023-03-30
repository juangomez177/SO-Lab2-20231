#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"

extern char *mypath[];

void execute_exit(int value){
	exit(0);
}

/*
void execute_cd(char *newpath){
	char *path = strtok_r(newpath, " ", &newpath);
	chdir(path);
}

*/


//Implementación del comando cd para cambiar la ruta del directorio actual,sólo debe recibir 1 argumento que será el cambio a la nnueva ruta
void execute_cd(char *newpath){
	printf("Variable newpath %s\n", newpath);
	char *path = strtok_r(newpath, " ", &newpath);
	if(path == NULL){
		printf("Error: el comando cd requiere un argumento\n");
	}else{
		if(strtok_r(NULL, " ", &newpath) != NULL){
			printf("Error: el comando cd solo acepta un argumento\n");
		}else{
			if (access(path, F_OK) == 0) {
				chdir(path);
				printf("La nueva ruta es %s\n", path);
			} else {
				printf("Directory does not exist: %s\n", path);
			}

			
		}
	}
}


//Implementación del comando path que contiene las rutas donde se buscarán los comandos ejecutables en el programa, 
//puede recibir cero(Se conserva el path actual) o más argumentos(se actualiza el path actual con el numero de argumentos que se ingresen)
void execute_path(char *newpath){
	printf("Variable newpath (%s)\n", newpath);
					
	//Path con cero argumentos
	if(strcmp(newpath, "") == 0){
	
		
		printf("Variable mypath sin modificaciones\n");
	
	//Path con n argumentos
	}else{
		/*
		
		
		int count = 1;  
		for (int i = 0; newpath[i] != '\0'; i++) {
			if (newpath[i] == ' ') {
			count++;
			}
		}
		
		// Redimensionar mypath con realloc
		int oldsize = PATH_SIZE;
		//mypath = realloc(mypath, (oldsize + count + 1) * sizeof(char*));
		
		// Separar newpath en particiones y copiarlas a mypath
		char *token = strtok(newpath, " ");
		int i = oldsize - 1;
		while (token != NULL) {
			i++;
			mypath[i] = token;
			token = strtok(NULL, " ");
		}
		
		i++;
		mypath[i] = "";

		*/
	}
}
