#include "shell.h"

void rtc_drive()
{
    char* rtc_driver = "/proc/driver/rtc";

    char line[32] = {'\0'};

    FILE *file = fopen (rtc_driver, "r");

    if(file != NULL)
    {
       for(int i = 0; i<2; i++) /* read a line */
       {
          fgets(line, 32, file);
          printf("%s", line+10);
       }

       fclose(file);
       printf("\n");
   }
}
void clock_def(char* argv1[], int argc1)
{
    if(argc1 != 5)
    {
        printf("clock: Usage: clock -t [interval] -n [duration]\n");
        return;
    }
    int int_len = strlen(argv1[2]);
    int dur_len = strlen(argv1[4]);

    for(int i = 0; i<int_len; i++)
    {
        if(!isdigit(argv1[2][i]))
        {
            printf("clock: Usage: clock -t [interval] -n [duration]\n");
            return;
        }
    }

    for(int i = 0; i<dur_len; i++)
    {
        if(!isdigit(argv1[4][i]))
        {
            printf("clock: Usage: clock -t [interval] -n [duration]\n");
            return;
        }
    }

    char* interval = (char*)malloc(1024);
    char* duration = (char*)malloc(1024);

    if(!strcmp(argv1[1],"-t")&&!strcmp(argv1[3],"-n"))
    {
        interval = argv1[2]; duration = argv1[4];
    }

    else
    {
        interval = argv1[4]; duration = argv1[2];
    }

    char** junk;
    long int interval_int = strtol(interval, junk, 10);
    long int duration_int = strtol(duration, junk, 10);

    long int n = duration_int/interval_int;

    for (size_t i = 0; i < n; i++)
    {
        rtc_drive();
        sleep(interval_int);
    }
    return;
}
