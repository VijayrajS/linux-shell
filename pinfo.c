#include "shell.h"

void pinfo(char* argv1[], int argc1, char* home)
{
    if(argc1 > 2)
    {
        printf("pinfo: Usage: pinfo [pid]\n");
        return;
    }

    int pid;
    char* pid_string = (char*) malloc(16);

    if(argc1 == 1)
    {
            pid = getpid();
            sprintf(pid_string, "%d", pid);
    }
    else
        pid_string = argv1[1];

    struct stat fileinfo;
    char* path = (char*) malloc(4096);

    strcpy(path, "/proc/");
    strcat(path, pid_string);
    strcat(path, "/stat");

    int file_exist = stat(path, &fileinfo);
    if(file_exist < 0)
    {
        printf("pinfo: Invalid pid\n");
        return;
    }


    char* file_buff = (char*) malloc(16384);

    int fd = open(path,O_RDONLY);
    int rd = read(fd, file_buff, 16384);

    char* split = strtok(file_buff, " \n\t");

    int i = 0;
    while(split)
    {
        if(i==0)
            printf("\tpid\t:%s\n", split);

        else if(i==2)
            printf("\tProcess Status\t:%s\n", split);

        else if(i==22)
            printf("\tVirtual Memory\t:%s\n", split);

        i++;
        split = strtok(NULL, " \n\t");
    }

    //Executable path
    char* exec_path = (char*) malloc(4096);
    char* readL_buf = (char*) malloc(4096);

    strcpy(exec_path, "/proc/");
    strcat(exec_path, pid_string);
    strcat(exec_path, "/exe");

    int exec_check = readlink(exec_path, readL_buf, 4095);
    readL_buf[4095] = '\0';

    if(exec_check<0)
    {
        printf("pinfo: Invalid pid or exe file access permission denied\n");
        return;
    }
    readL_buf = ishome(home, readL_buf);
    printf("\tExecutable Path\t: %s\n", readL_buf);
    return;
}
