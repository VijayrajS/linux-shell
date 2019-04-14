#include "shell.h"

void redir(char* argv1[], int argc1,  char* home)
{
    int inp_r = 0, out_r = 0;   // Input and output redirect flags
    int fd_in = 0, fd_out = 1;       // Denotes the two file descriptors which will be used

    for (int i = 0; i < argc1; i++)
    {
        if(!strcmp(argv1[i], "<"))
        {
            inp_r = 1;
            if(i < argc1-1)
            {
                fd_in = open(argv1[++i], O_RDONLY);
                if(fd_in == -1)
                {
                    perror("File Error");
                    return;
                }
            }
            else
            {
                printf("csh: no input file found\n");
                return;
            }

        }
        else if(!strcmp(argv1[i], ">"))
        {
            out_r = 1;
            if(i < argc1-1)
                fd_out = open(argv1[++i], O_WRONLY | O_CREAT, 0755);

            else
            {
                printf("csh: no output file found\n" );
                return;
            }
        }

        else if(!strcmp(argv1[i], ">>"))
        {
            out_r = 1;
            if(i < argc1-1)
                fd_out = open(argv1[++i], O_WRONLY | O_APPEND | O_CREAT, 0755);
            else
            {
                printf("csh: no output file found\n" );
                return;
            }
        }
    }

    char* argv2[argc1];
    int j = 0;

    for (int i = 0; i < argc1; i++)
    {
        if(!strcmp(argv1[i], ">")|| !strcmp(argv1[i], "<")|| !strcmp(argv1[i], ">>")) break;
        else
        {
            argv2[j] = (char*) malloc(4096);
            strcpy(argv2[j++], argv1[i]);
        }
    }

    int temp_i, temp_o;

    if(inp_r)
    {
        temp_i = dup(0);
        if(dup2(fd_in, 0) == -1)
        {
            perror("RedirError");
            return;
        }
    }

    if(out_r)
    {
        temp_o = dup(1);
        if(dup2(fd_out, 1) == -1)
        {
            perror("RedirError");
            return;
        }
    }

    int background = 0;                              // Flag for background
    if(!strcmp(argv2[j-1], "&"))
    {
        background = 1;
        strcpy(argv2[j-1], "\0");
    }

    if(!strcmp(argv2[0], "echo") && background != 1)
        echo(argv2, j);
    else if(!strcmp(argv2[0], "pwd") && background != 1)
        pwd();
    else if(!strcmp(argv2[0], "ls") && background != 1)
        ls(argv2, j, home);
    else if(!strcmp(argv2[0], "remindme") && background != 1)
        reminder(argv2, j);
    else if(!strcmp(argv2[0], "pinfo") && background != 1)
        pinfo(argv2, j, home);
    else if(!strcmp(argv2[0], "clock") && background != 1)
        clock_def(argv2, j);
    else if(!strcmp(argv2[0], "setenv"))
        seten(argv2, j);
    else if(!strcmp(argv2[0], "overkill") && background != 1)
       okill();
    else if(!strcmp(argv2[0], "unsetenv"))
        unseten(argv2, j);
    else if(!strcmp(argv2[0], "jobs"))
       jobs();
    else if(!strcmp(argv2[0], "kjob"))
       kjob(argv2, j);
    else if(!strcmp(argv2[0], "bg"))
       bg(argv2, j);
    else if(!strcmp(argv2[0], "fg"))
       fg(argv2, j);
    else
    {
        argv2[j]=NULL;
        exec(argv2, j, background);
    }

    if(inp_r) dup2(temp_i, 0);
    if(out_r) dup2(temp_o, 1);
}
