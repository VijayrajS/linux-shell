#include "shell.h"

int reminder(char* argv1[], int argc1)
{
    if(argc1<3)
    {
        printf("remindme: Usage: remindme [time(s)] [reminder]\n");
        return 1;
    }
    int time_len = strlen(argv1[1]);

    for(int i = 0; i<time_len; i++)
    {
        if(!isdigit(argv1[1][i]))
        {
            printf("remindme: Usage: remindme [time(s)] [reminder]\n");
            return 1;
        }
    }

    char** junk;
    long int timer = strtol(argv1[1], junk, 10);

    int pid = fork();
    if(pid == 0)
    {
        sleep(timer);
        printf("\nReminder: ");
        for (int i = 2; i < argc1; i++)
            printf("%s ",argv1[i]);
        printf("\n");
    }
    return 0;
}
