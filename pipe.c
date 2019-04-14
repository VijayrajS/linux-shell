#include "shell.h"

int pipe_array[2];
extern int shell_pid;

void pipe_exec(char* argv1[], int argc1, char* home)
{
    int pd_in = 0, pd_out = 1;
    int stdin  = dup(0);
    int stdout = dup(1);

    for(int i = 0; i < argc1; i++)
    {
        /* Command tokenisation wrt whitespace */
        char* split = strtok(argv1[i], " \t\n");
        char* cmd[4096];
        for (int e = 0; e < 4096; e++) cmd[e] = NULL;

        int word_count = 0;
        while(split)
        {
            cmd[word_count++] = split;
            split = strtok(NULL, " \n\t");
        }

        /* * * * * * * * * * * * * * * * * * * * */
        if(dup2(pd_in, 0) == -1)
        {
            perror("PipeDup2Error");
            return;
        }

        if(pipe(pipe_array) != 0)
            perror("pipe_error");

        close(pd_in);

        dup2(pipe_array[1], 1);
        close(pipe_array[1]);

        if(i == argc1 - 1)
        {
            if(dup2(stdout, 1) == -1)
            {
                perror("PipeDup2Error");
                return;
            }
        }

        //Execute
        int redir_flag = 0;
        for (int q = 0; q < word_count; q++)
        {
            if(!strcmp(cmd[q], ">")|| !strcmp(cmd[q], "<") || !strcmp(cmd[q], ">>"))
            {
                redir_flag = 1;
                redir(cmd, word_count, home);
                break;
            }
        }

        if(redir_flag)
        {
            pd_in = pipe_array[0];
            continue;
        }

        int background = 0;                              // Flag for background
        if(!strcmp(cmd[word_count-1], "&"))
        {
            background = 1;
            strcpy(cmd[word_count-1], "\0");
        }

        if(!strcmp(cmd[0], "echo") && background != 1)
            echo(cmd, word_count);
        else if(!strcmp(cmd[0], "pwd") && background != 1)
            pwd();
        else if(!strcmp(cmd[0], "cd") && background != 1)
            cd(cmd, word_count, home);
        else if(!strcmp(cmd[0], "ls") && background != 1)
            ls(cmd, word_count, home);
        else if(!strcmp(cmd[0], "remindme") && background != 1)
            reminder(cmd, word_count);
        else if(!strcmp(cmd[0], "pinfo") && background != 1)
            pinfo(cmd, word_count, home);
        else if(!strcmp(cmd[0], "clock") && background != 1)
            clock_def(cmd, word_count);
        else if(!strcmp(cmd[0], "setenv"))
            seten(cmd, word_count);
        else if(!strcmp(cmd[0], "overkill") && background != 1)
           okill();
        else if(!strcmp(cmd[0], "unsetenv"))
            unseten(cmd, word_count);
        else if(!strcmp(cmd[0], "jobs"))
           jobs();
        else if(!strcmp(cmd[0], "kjob"))
           kjob(cmd, word_count);
        else if(!strcmp(cmd[0], "bg"))
           bg(cmd, word_count);
        else if(!strcmp(cmd[0], "fg"))
           fg(cmd, word_count);
        else if(!strcmp(cmd[0], "quit"))
            kill(shell_pid, SIGKILL);
        else
            exec(cmd, word_count, background);

        pd_in = pipe_array[0];
    }
    dup2(stdin, 0);
    close(stdin);
    dup2(stdout, 1);
    close(stdout);

}
