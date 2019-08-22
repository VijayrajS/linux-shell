#include "shell.h"

extern int curr_pid;

int time_counter = 0;

pid_det pid_arr[2048];

void init()
{
    for (int i = 0; i < 2048; i++)
    {
        pid_arr[i].id = 0;
        pid_arr[i].crtime = INT_MAX;
    }
}

int proc_comp(const void *a, const void *b)
{
    pid_det *ia = (pid_det*)a;
    pid_det *ib = (pid_det*)b;
    return ia->crtime - ib->crtime;
}

void fin_bg_proc()                     // Message for exiting background process
{
    int pid;
    int status;
    for (int i = 0; i < 2048; i++)
    {
        if(pid_arr[i].id != 0)
        {
            pid = waitpid(pid_arr[i].id, &status, WNOHANG);
            if(pid)
            {
                fprintf(stderr,"shell: process with PID %d exited normally\n", pid_arr[i].id);
                pid_arr[i].id = 0;
                pid_arr[i].crtime = -1;
                break;
            }
        }
    }
}

void exec(char* argv1[], int argc1, int bg) // FG ansd BG processes
{
    int pid = fork();
    curr_pid= pid;

    int status;

    if(pid == -1)
    {
        perror("Fork Error");
        return;
    }

    if(pid == 0)                        // Child process
    {
        if(bg)
        {
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);
            setpgid(getpid(), pid);
        }
        int err_check = execvp(argv1[0], argv1);
        if(err_check != 0)
        {
            perror("Error");
            return;
        }
    }

    else
    {
        if(bg == 0)
        {
            int status;
            waitpid(pid, NULL, WUNTRACED);
        }

        if(bg == 1)
        {
            for(int i=0; i<2048; ++i)
            {
                if(pid_arr[i].id==0)
                {
                    pid_arr[i].id = pid;
                    pid_arr[i].crtime = time_counter++;
                    strcpy(pid_arr[i].name, argv1[0]);
                    break;
                }
            }
        }
    }
    return;
}

void okill()                                    // Overkill
{
    for (int i = 0; i < 2048; i++)
        if(pid_arr[i].id != 0)
        {
            kill(pid_arr[i].id, 9);
            pid_arr[i].id = 0;
            pid_arr[i].crtime = -1;
        }
    printf("overkill: Overkill successful\n");
}

void jobs()
{
    qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);
    int j = 0;
	for(int i=0;i<2048;i++)
	{
        if(pid_arr[i].id != 0)
        {
            printf("[%d] ", ++j);

            char pidstr[15];
            sprintf(pidstr,"%d", pid_arr[i].id);
            char* path = (char*) malloc(4096);

            strcpy(path, "/proc/");
            strcat(path, pidstr);
            strcat(path, "/stat");

            char* file_buff = (char*) malloc(16384);

            int fd = open(path,O_RDONLY);
            int rd = read(fd, file_buff, 16384);

            char* split = strtok(file_buff, " \n\t");

            int i1 = 0;
            while(split)
            {
                if(i1==2)
                {
                    char* st[1];
                    st[0] = split;

                    if(!strcmp(st[0],"R"))
                        printf("running ");
                    else if(!strcmp(st[0],"D"))
                        printf("sleeping ");
                    else if(!strcmp(st[0],"S"))
                        printf("sleeping ");
                    else if(!strcmp(st[0],"Z"))
                        printf("zombie ");
                    else if(!strcmp(st[0],"T"))
                        printf("stopped ");

                    break;
                }

                i1++;
                split = strtok(NULL, " \n\t");
            }

            printf("%s[%d]\n", pid_arr[i].name, pid_arr[i].id);
        }
	}
}

void kjob(char* argv1[], int argc1)
{
    if(argc1!=3)
    {
        printf("kjob: Usage: kjob <job_number> <signal_number>\n");
        return;
    }

    int i;
    for(i=0;i<strlen(argv1[1]);i++)
    {
        if(!isdigit(argv1[1][i]))
        {
            printf("kjob: Usage: kjob <job_number> <signal_number>\n");
            return;
        }
    }

    for(i=0;i<strlen(argv1[2]);i++)
    {
        if(!isdigit(argv1[2][i]))
        {
            printf("kjob: Usage: kjob <job_number> <signal_number>\n");
            return;
        }
    }
    qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);
    char** junk;
    long int job_num = strtol(argv1[1], junk, 10);
    long int job_sig = strtol(argv1[2], junk, 10);

    if(!job_num || job_num > 2048 || !pid_arr[job_num-1].id)
    {
        printf("kjob: invalid job number\n");
        return;
    }

    kill(pid_arr[job_num-1].id, job_sig);
}

void bg(char* argv1[], int argc1)
{
    if(argc1!=2)
    {
        printf("bg: Usage: bg <job_number>\n");
        return;
    }
    int i;
    for(i=0;i<strlen(argv1[1]);i++)
    {
        if(!isdigit(argv1[1][i]))
        {
            printf("bg: Usage: bg <job_number>\n");
            return;
        }
    }

    qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);

    char** junk;
    long int job_num = strtol(argv1[1], junk, 10);

    if(!job_num || job_num > 2048 || !pid_arr[job_num-1].id)
    {
        printf("bg: invalid job number\n");
        return;
    }
    printf("[+] %d %s\n", pid_arr[job_num-1].id, pid_arr[job_num-1].name);
    kill(pid_arr[job_num-1].id, SIGCONT);
}

void fg(char* argv1[], int argc1)
{
    if(argc1!=2)
    {
        printf("fg: Usage: fg <job_number>\n");
        return;
    }

    int i;
    for(i=0;i<strlen(argv1[1]);i++)
    {
        if(!isdigit(argv1[1][i]))
        {
            printf("fg: Usage: fg <job_number>\n");
            return;
        }
    }

    qsort(pid_arr, 2048, sizeof(pid_det), proc_comp);

    char** junk;
    long int job_num = strtol(argv1[1], junk, 10);

    int j = job_num - 1;

    if(!job_num || job_num > 2048 || !pid_arr[j].id)
    {
        printf("fg: invalid job number\n");
        return;
    }

    int fg_pid = pid_arr[j].id, status;
    curr_pid =  pid_arr[j].id;

    pid_arr[j].id = 0;
    pid_arr[j].crtime = INT_MAX;


    kill(fg_pid, SIGCONT);
    waitpid(fg_pid, &status, WUNTRACED);

    return;
}
