#ifndef __WISH_U_H__
#define __WISH_U_H__

//extern char *mypath[];
//extern char error_message[30] = "An error has occurred\n";

void execute_exit(char *args);
void execute_cd(char* newpath);
void execute_path(char *newpaths, char ***mypath);

#endif
