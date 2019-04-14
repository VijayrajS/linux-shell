#include "shell.h"

int cd(char *argv1[], int argc1, char* home)
{
    char err[1005] = {'\0'};

    if(argc1 == 1|| !strcmp(argv1[1],"~"))
    {
        chdir(home);
        return 0;
    }

    if(argc1>2)
    {
        printf("cd: too many arguements given. Usage: cd [directory]\n");
        return 1;
    }

    int cd_check;

    if(argv1[1][0] == '~')
    {
        char* string_with_home = (char*) malloc(255);
        strcpy(string_with_home, home);
        strcat(string_with_home, argv1[1]+1);       // In case there is a ~/... path
        cd_check = chdir(string_with_home);
    }
    else
        cd_check = chdir(argv1[1]);

    if(cd_check!=0)
    {
      strcpy(err, "bash: cd: ");
	  strcat(err, argv1[1]);
      perror(err);
   }
   return 0;
}
