#include "shell.h"

int echo(char *argv1[], int argc1)
{
    if(argc1 == 1)          //No arguements given
        ;

    else
    {
        for(int i = 1; i<argc1 ; i++)
        {
            if(argv1[i][0] == '$')
            {
                char* env_var_val = getenv(argv1[i]+1);

                if(env_var_val!=NULL)
                    printf("%s", env_var_val);

                else {
                    printf("NULL\n");
                    continue;
                }
            }
            else
                for(int j = 0; argv1[i][j]; j++)
                {
                    if(argv1[i][j]!='"' && argv1[i][j] !='\'')
                        printf("%c",argv1[i][j]);
                }
            if(i != argc1 - 1)
                printf(" ");
        }
    }

    printf("\n");
    return 0;
}
