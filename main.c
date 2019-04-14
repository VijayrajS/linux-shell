#include "shell.h"

int curr_pid, shell_pid;
extern pid_det pid_arr[2048];
extern int time_counter;

const char* get_process_name_by_pid(const int pid)
{
    char* name = (char*)malloc(1024);
    if(name)
    {
        sprintf(name, "/proc/%d/cmdline",pid);
        FILE* f = fopen(name,"r");
        if(f)
        {
            size_t size;
            size = fread(name, sizeof(char), 1024, f);
            if(size>0)
            {
                if('\n'==name[size-1])
                    name[size-1]='\0';
            }
            fclose(f);
        }
    }
    return name;
}

char* ishome(char* home, char* path)                 //Path resolution on prompt
{
    char* final_path = (char*) malloc (4096);
    int path_length = strlen(path);
    int home_length = strlen(home);
    int i = 0;

    for(; i<home_length; i++)
    {
        if(path[i] != home[i])
            return path;
    }

    final_path[0] = '~';

    if(strcmp(home,path))
    {
        for(;i<path_length;i++)
            final_path[i-home_length+1] = path[i];
    }

    final_path[path_length - home_length + 1] = '\0';

    return final_path;
}

void sigint_handler(int sig)
{
    if(getpid()!=shell_pid)
        return;

    if(curr_pid != -1)
        kill(curr_pid, SIGINT);

    signal(SIGINT, sigint_handler);
}

void sigstp_handler()
{
    if(getpid()!=shell_pid)
        return;

	if(curr_pid != -1)
    {
		kill(curr_pid, SIGTTIN);
		kill(curr_pid, SIGTSTP);
        int i;
        for(i=0; i<2048; ++i)
        {
            if(pid_arr[i].id==0)
            {
                pid_arr[i].id = curr_pid;
                pid_arr[i].crtime = time_counter++;
                strcpy(pid_arr[i].name, get_process_name_by_pid(curr_pid));
                break;
            }
        }

        printf("[+] stopped %d %s\n" , curr_pid, pid_arr[i].name);
	}
    signal(SIGTSTP, sigstp_handler);
}

void bash_loop()
{
    char launch_home[4096] = {'\0'};        // Directory from where home is launched
    char* cwd = (char*) malloc (4096);
    char* cwd_f = (char*) malloc (4096);
    getcwd(launch_home,4096);
    init();

    curr_pid = -1;

    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, sigstp_handler);

    shell_pid = getpidf();
    setpgid(shell_pid, shell_pid);
    tcsetpgrp(0, shell_pid);

    while(1)
    {
        curr_pid = -1;
        fin_bg_proc();
        struct utsname sys_name;  // To get system name
        char* u_name;             // To get user name


        int s_check = uname(&sys_name);
        u_name = getenv("USER");

        if(s_check < 0)
        {
            printf("cmd display unsucceseful\n");
            exit(1);
        }

        getcwd(cwd,4096);
        cwd_f = ishome(launch_home, cwd);
        printf("<%s@%s:%s>", u_name, sys_name.nodename, cwd_f);

        /*************Taking input and split*******/

        char* cmd_line_m = NULL;                           // Input buffer
        size_t bufr_size_m = 0;                            // getLine will allocate a buffer for us

        getline(&cmd_line_m, &bufr_size_m, stdin);

        char* cmd_array_m[100000];
        for (int e = 0; e < 100000; e++) cmd_array_m[e] = NULL;

        char* splitter_m = strtok(cmd_line_m, ";");
        int line_cnt = 0;

        while(splitter_m)
        {
            cmd_array_m[line_cnt++] = splitter_m;
            splitter_m = strtok(NULL, ";");
        }

        if(!strcmp(cmd_line_m,"\n"))                         // Empty command
            continue;

        for(int i = 0; i < line_cnt; i++)
        {
            char* cmd_line = cmd_array_m[i];                   // Input buffer

            if(!strcmp(cmd_line,"\n"))                       // Empty command
                continue;

            //------------------------------------------------------------------

            char* splitter_p = strtok(cmd_line, "|\n");
            char* cmd_array_p[100000];
            for (int e = 0; e < 100000; e++) cmd_array_p[e] = NULL;

            int pipe_flag = 0;
            int cmd_count = 0;

            while(splitter_p)
            {
                cmd_array_p[cmd_count++] = splitter_p;
                splitter_p = strtok(NULL, "|\n");
            }

            if(!cmd_array_p[0])                                // Empty command
                continue;

            if(cmd_count > 1)
            {
                pipe_exec(cmd_array_p, cmd_count, launch_home);
                pipe_flag = 1;
            }

            if(pipe_flag) continue;

            //------------------------------------------------------------------
            char* splitter = strtok(cmd_line, " \t\n");
            char* cmd_array[100000];
            for (int e = 0; e < 100000; e++) cmd_array[e] = NULL;

            int word_count = 0;

            while(splitter)
            {
                cmd_array[word_count++] = splitter;
                splitter = strtok(NULL, " \n\t");
            }

            if(!cmd_array[0])                                // Empty command
                continue;
            /****************************************/

            /*******Command identification***********/


            int redir_flag = 0;
            for (int q = 0; q < word_count; q++)
            {
                if(!strcmp(cmd_array[q], ">")|| !strcmp(cmd_array[q], "<") || !strcmp(cmd_array[q], ">>"))
                {
                    redir_flag = 1;
                    redir(cmd_array, word_count, launch_home);
                    break;
                }
            }

            if(redir_flag) continue;

            int background = 0;                              // Flag for background
            if(!strcmp(cmd_array[word_count-1], "&"))
            {
                background = 1;
                strcpy(cmd_array[word_count-1], "\0");
            }

            if(!strcmp(cmd_array[0], "echo") && background != 1)
                echo(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "pwd") && background != 1)
                pwd();
            else if(!strcmp(cmd_array[0], "cd") && background != 1)
                cd(cmd_array, word_count, launch_home);
            else if(!strcmp(cmd_array[0], "ls") && background != 1)
                ls(cmd_array, word_count, launch_home);
            else if(!strcmp(cmd_array[0], "remindme") && background != 1)
                reminder(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "pinfo") && background != 1)
                pinfo(cmd_array, word_count, launch_home);
            else if(!strcmp(cmd_array[0], "clock") && background != 1)
                clock_def(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "setenv"))
                seten(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "overkill") && background != 1)
               okill();
            else if(!strcmp(cmd_array[0], "unsetenv"))
                unseten(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "jobs"))
               jobs();
            else if(!strcmp(cmd_array[0], "kjob"))
               kjob(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "bg"))
               bg(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "fg"))
               fg(cmd_array, word_count);
            else if(!strcmp(cmd_array[0], "quit"))
                kill(shell_pid, SIGKILL);
            else
                exec(cmd_array, word_count, background);
        }
    }
}
int main(int argc, char const *argv[])
{
    bash_loop();
    return 0;
}
