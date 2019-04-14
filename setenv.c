#include "shell.h"

void seten(char* argv1[], int argc1)
{
    if(argc1 == 3 || argc1 == 2)
    {
        char* new_val = (char*)malloc(4096);

        if(argc1 == 3)
            new_val = argv1[2];

        else
            strcpy(new_val,"");
            
        setenv(argv1[1], new_val, 1);
    }
    else
    {
        printf("setenv: Usage: setenv VAR [value]\n");
        return;
    }
}

void unseten(char* argv1[], int argc1)
{
    if(argc1 == 2)
        unsetenv(argv1[1]);

    else
    {
        printf("unsetenv: Usage: unsetenv VAR\n");
        return;
    }
}
